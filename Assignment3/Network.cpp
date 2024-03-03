#include <fstream>
#include "Network.h"
#include <sstream>
#include "Packet.h"
#include <stack>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    for (string s: commands) {
        vector<std::string> command_words;
        std::istringstream iss(s);
        std::string word;
        std::string message_text;
        while (iss >> word) {
            if (!word.empty()) {
                if (word.find('#') != std::string::npos) {
                    std::getline(iss, message_text);
                    message_text = word + message_text;
                    command_words.push_back(message_text);
                    break;
                } else {
                    command_words.push_back(word);
                }
            }

        }

        int dash_num = 9 + s.length();
        std::string dashes;
        for (int i = 0; i < dash_num; ++i) {
            dashes += "-";
        }

        if (command_words[0] == "MESSAGE") {
            string sender = command_words[1];
            string receiver = command_words[2];
            std::string message;
            for (char c: command_words[3]) {
                if (c != '#') {
                    message += c;
                }
            }

            std::vector<std::string> parts;

            for (int i = 0; i < message.length(); i += message_limit) {
                std::string part = message.substr(i, message_limit);
                parts.push_back(part);
            }

            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;
            std::cout << "Message to be sent: " << "\"" << message << "\"" << endl;
            std::cout << endl;

            for (int i = 0; i < parts.size(); i++) {
                std::string mac_adress_1;
                std::string mac_adress_2;
                std::string ip1;
                std::string ip2;
                std::string forward;

                for (int j = 0; j < client_num; ++j) {
                    if (clients[j].client_id == sender) {
                        ip1 = clients[j].client_ip;
                    }
                    if (clients[j].client_id == receiver) {
                        ip2 = clients[j].client_ip;
                    }
                    if (clients[j].client_id == sender) {
                        mac_adress_1 = clients[j].client_mac;
                        for (pair<const string, string> map: clients[j].routing_table) {
                            if (map.first == receiver) {
                                forward = map.second;
                            }
                        }
                    }
                }

                for (int j = 0; j < client_num; ++j) {
                    if (clients[j].client_id == forward) {
                        mac_adress_2 = clients[j].client_mac;
                    }
                }

                ApplicationLayerPacket *applicationLayerPacket = new ApplicationLayerPacket(0, sender,
                                                                                            receiver,
                                                                                            parts[i]);
                TransportLayerPacket *transportLayerPacket = new TransportLayerPacket(1, sender_port, receiver_port);
                NetworkLayerPacket *networkLayerPacket = new NetworkLayerPacket(2, ip1, ip2);
                PhysicalLayerPacket *physicalLayerPacket = new PhysicalLayerPacket(3, mac_adress_1, mac_adress_2);

                stack<Packet *> out;
                out.push(applicationLayerPacket);
                out.push(transportLayerPacket);
                out.push(networkLayerPacket);
                out.push(physicalLayerPacket);

                for (int j = 0; j < client_num; j++) {
                    if (clients[j].client_id == sender) {
                        clients[j].outgoing_queue.push(out);
                    }
                }
                vector_stack.push_back(out);

                std::cout << "Frame " << "#" << i + 1 << endl;
                physicalLayerPacket->print();
                networkLayerPacket->print();
                transportLayerPacket->print();
                applicationLayerPacket->print();
                std::cout << "Message chunk carried: " << "\"" << parts[i] << "\"" << endl;
                std::cout << "Number of hops so far: " << 0 << endl;
                std::cout << "--------" << endl;
            }

        } else if (command_words[0] == "SHOW_FRAME_INFO") {

            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;

            for (int i = 0; i < client_num; ++i) {
                if (clients[i].client_id == command_words[1]) {
                    queue<stack<Packet *>> frames;
                    if (command_words[2] == "out") {
                        frames = clients[i].outgoing_queue;
                    } else if (command_words[2] == "in") {
                        frames = clients[i].incoming_queue;
                    }
                    if (std::stoi(command_words[3]) - 1 > frames.size() || frames.empty()) {
                        std::cout << "No such frame." << endl;
                    } else {
                        std::cout << "Current Frame #" << command_words[3] << " " << "on the "
                                  << (command_words[2] == "out" ? "outgoing" : "incoming") << " queue of client "
                                  << command_words[1] << endl;
                        for (int j = 0; j < std::stoi(command_words[3]) - 1; ++j) {
                            frames.pop();
                        }
                        stack<Packet *> packets = frames.front();
                        PhysicalLayerPacket *layer_3 = dynamic_cast<PhysicalLayerPacket *>(packets.top());
                        packets.pop();
                        NetworkLayerPacket *layer_2 = dynamic_cast<NetworkLayerPacket *>(packets.top());
                        packets.pop();
                        TransportLayerPacket *layer_1 = dynamic_cast<TransportLayerPacket *>(packets.top());
                        packets.pop();
                        ApplicationLayerPacket *layer_0 = dynamic_cast<ApplicationLayerPacket *>(packets.top());
                        std::cout << "Carried Message: " << "\"" << layer_0->message_data << "\"" << endl;
                        std::cout << "Layer 0 info: ";
                        layer_0->print();
                        std::cout << "Layer 1 info: ";
                        layer_1->print();
                        std::cout << "Layer 2 info: ";
                        layer_2->print();
                        std::cout << "Layer 3 info: ";
                        layer_3->print();

                        int num_of_hops_print = 0;
                        for (int j = 0; j < clients[i].log_entries.size(); ++j) {
                            if (!clients[i].log_entries[j].message_content.empty() && !layer_0->message_data.empty()) {
                                if (clients[i].log_entries[j].message_content.find(layer_0->message_data) !=
                                    std::string::npos) {
                                    if (clients[i].log_entries[j].number_of_hops > num_of_hops_print) {
                                        num_of_hops_print = clients[i].log_entries[j].number_of_hops;
                                    }

                                }
                            }
                        }
                        if (command_words[2] == "in") {
                            std::cout << "Number of hops so far: " << num_of_hops_print + 1 << endl;
                        } else {
                            std::cout << "Number of hops so far: " << num_of_hops_print << endl;
                        }

                    }

                }
            }

        } else if (command_words[0] == "SHOW_Q_INFO") {
            for (int i = 0; i < client_num; ++i) {
                if (clients[i].client_id == command_words[1]) {
                    queue<stack<Packet *>> frames;
                    if (command_words[2] == "in") {
                        frames = clients[i].incoming_queue;
                    } else if (command_words[2] == "out") {
                        frames = clients[i].outgoing_queue;
                    }
                    std::cout << dashes << endl;
                    std::cout << "Command: " << s << endl;
                    std::cout << dashes << endl;
                    std::cout << "Client " << command_words[1] << " "
                              << (command_words[2] == "out" ? "Outgoing" : "Incoming") << " Queue Status" << endl;
                    std::cout << "Current total number of frames: " << frames.size() << endl;


                }

            }
        } else if (command_words[0] == "SEND") {
            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;
            for (int i = 0; i < client_num; ++i) {
                if (!clients[i].outgoing_queue.empty()) {
                    std::string message;
                    int frame_index = 1;
                    while (!clients[i].outgoing_queue.empty()) {
                        string to_client;
                        stack<Packet *> packets = clients[i].outgoing_queue.front();
                        stack<Packet *> push_packets = clients[i].outgoing_queue.front();
                        PhysicalLayerPacket *layer_3 = dynamic_cast<PhysicalLayerPacket *>(packets.top());
                        packets.pop();
                        NetworkLayerPacket *layer_2 = dynamic_cast<NetworkLayerPacket *>(packets.top());
                        packets.pop();
                        TransportLayerPacket *layer_1 = dynamic_cast<TransportLayerPacket *>(packets.top());
                        packets.pop();
                        ApplicationLayerPacket *layer_0 = dynamic_cast<ApplicationLayerPacket *>(packets.top());

                        for (int j = 0; j < client_num; ++j) {
                            if (clients[j].client_mac == layer_3->receiver_MAC_address) {
                                clients[j].incoming_queue.push(push_packets);
                                to_client = clients[j].client_id;
                            }
                        }

                        std::cout << "Client " << clients[i].client_id << " sending frame #" << frame_index
                                  << " to client " << to_client << endl;
                        layer_3->print();
                        layer_2->print();
                        layer_1->print();
                        layer_0->print();

                        std::cout << "Message chunk carried: " << "\"" << layer_0->message_data << "\""
                                  << endl;
                        message += layer_0->message_data;


                        if (!layer_0->message_data.empty()) {
                            if (layer_0->message_data.find('.') != std::string::npos ||
                                layer_0->message_data.find('!') != std::string::npos ||
                                layer_0->message_data.find('?') != std::string::npos) {

                                int num_of_hops = 0;
                                for (int j = 0; j < client_num; ++j) {
                                    if (!clients[j].log_entries.empty()) {
                                        for (int k = 0; k < clients[j].log_entries.size(); ++k) {
                                            if (clients[j].log_entries[k].message_content == message) {
                                                if (clients[j].log_entries[k].number_of_hops + 1 > num_of_hops) {
                                                    num_of_hops = clients[j].log_entries[k].number_of_hops + 1;
                                                }

                                            }

                                        }
                                    }

                                }

                                if (!clients[i].log_entries.empty()) {
                                    if (clients[i].log_entries.back().activityTypeToString() != "Message Forwarded") {

                                        time_t currentTime = time(nullptr);
                                        struct tm *timeinfo = localtime(&currentTime);
                                        char buffer[20];
                                        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                                        Log sent_log(buffer, message, frame_index, num_of_hops,
                                                     clients[i].client_id, layer_0->receiver_ID, true,
                                                     ActivityType(ActivityType::MESSAGE_SENT));
                                        clients[i].log_entries.push_back(sent_log);
                                    }
                                } else {
                                    time_t currentTime = time(nullptr);
                                    struct tm *timeinfo = localtime(&currentTime);
                                    char buffer[20];
                                    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                                    Log sent_log(buffer, message, frame_index, num_of_hops, clients[i].client_id,
                                                 layer_0->receiver_ID, true,
                                                 ActivityType(ActivityType::MESSAGE_SENT));
                                    clients[i].log_entries.push_back(sent_log);
                                }

                                frame_index = 1;
                                message.clear();
                                int num_of_hops_print = 0;
                                for (int j = 0; j < clients[i].log_entries.size(); ++j) {
                                    if (!clients[i].log_entries[j].message_content.empty() &&
                                        !layer_0->message_data.empty()) {
                                        if (clients[i].log_entries[j].message_content.find(layer_0->message_data) !=
                                            std::string::npos) {
                                            num_of_hops_print = clients[i].log_entries[j].number_of_hops;
                                        }
                                    }

                                }
                                std::cout << "Number of hops so far: " << num_of_hops_print + 1 << endl;
                                std::cout << "--------" << endl;
                            } else {
                                int num_of_hops = 0;
                                for (int j = 0; j < clients[i].log_entries.size(); ++j) {
                                    if (!clients[i].log_entries[j].message_content.empty() &&
                                        !layer_0->message_data.empty()) {
                                        if (clients[i].log_entries[j].message_content.find(layer_0->message_data) !=
                                            std::string::npos) {
                                            num_of_hops = clients[i].log_entries[j].number_of_hops;
                                        }
                                    }

                                }
                                std::cout << "Number of hops so far: " << num_of_hops + 1 << endl;
                                std::cout << "--------" << endl;

                                frame_index += 1;
                            }
                        }

                        clients[i].outgoing_queue.pop();


                    }
                }


            }
        } else if (command_words[0] == "RECEIVE") {
            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;
            for (int i = 0; i < client_num; ++i) {
                if (!clients[i].incoming_queue.empty()) {
                    int frame_index = 1;
                    std::string message;
                    while (!clients[i].incoming_queue.empty()) {
                        bool is_there_a_reciever = false;
                        string to_client;
                        stack<Packet *> packets = clients[i].incoming_queue.front();
                        stack<Packet *> push_packets = clients[i].incoming_queue.front();
                        PhysicalLayerPacket *layer_3 = dynamic_cast<PhysicalLayerPacket *>(packets.top());
                        packets.pop();
                        NetworkLayerPacket *layer_2 = dynamic_cast<NetworkLayerPacket *>(packets.top());
                        packets.pop();
                        TransportLayerPacket *layer_1 = dynamic_cast<TransportLayerPacket *>(packets.top());
                        packets.pop();
                        ApplicationLayerPacket *layer_0 = dynamic_cast<ApplicationLayerPacket *>(packets.top());
                        packets.pop();

                        message += layer_0->message_data;

                        std::string before_id;
                        bool message_comp = false;

                        for (int j = 0; j < client_num; ++j) {
                            if (clients[j].client_mac == layer_3->sender_MAC_address) {
                                before_id = clients[j].client_id;
                            }
                        }
                        if (layer_0->receiver_ID == clients[i].client_id) {
                            int num_of_hops = 0;
                            for (int j = 0; j < client_num; ++j) {
                                if (clients[j].client_mac == layer_3->sender_MAC_address) {
                                    for (int k = 0; k < clients[j].log_entries.size(); ++k) {
                                        if (!clients[j].log_entries[k].message_content.empty() &&
                                            !layer_0->message_data.empty()) {
                                            if (clients[j].log_entries[k].message_content.find(layer_0->message_data) !=
                                                std::string::npos) {
                                                num_of_hops = clients[j].log_entries[k].number_of_hops;
                                            }
                                        }


                                    }
                                }

                            }
                            message_comp = true;
                            std::cout << "Client " << clients[i].client_id << " receiving frame #" << frame_index
                                      << " from client " << before_id << ", originating from client "
                                      << layer_0->sender_ID << endl;
                            layer_3->print();
                            layer_2->print();
                            layer_1->print();
                            layer_0->print();
                            std::cout << "Message chunk carried: " << "\"" << layer_0->message_data << "\"" << endl;

                            std::cout << "Number of hops so far: " << num_of_hops + 1 << endl;
                            std::cout << "--------" << endl;


                        } else {


                            std::string betwwen_reciever_MAC;
                            for (int j = 0; j < client_num; ++j) {
                                if (clients[i].client_id == clients[j].client_id) {
                                    for (pair<const string, string> map: clients[j].routing_table) {
                                        if (map.first == layer_0->receiver_ID) {
                                            for (int k = 0; k < client_num; ++k) {
                                                if (clients[k].client_id == map.second) {
                                                    betwwen_reciever_MAC = clients[k].client_mac;
                                                }
                                            }
                                        }

                                    }
                                }

                            }
                            if (frame_index == 1 && !betwwen_reciever_MAC.empty()) {

                                std::cout << "Client " << clients[i].client_id << " receiving a message"
                                          << " from client "
                                          << before_id
                                          << ", but intended for client " << layer_0->receiver_ID << ". Forwarding... "
                                          << endl;
                            } else if (frame_index == 1 && betwwen_reciever_MAC.empty()) {
                                std::cout << "Client " << clients[i].client_id << " receiving frame #"
                                          << frame_index
                                          << " from client "
                                          << before_id
                                          << ", but intended for client " << layer_0->receiver_ID
                                          << ". Forwarding... "
                                          << endl;
                            }

                            if (!betwwen_reciever_MAC.empty()) {

                                string sender_Mac = layer_3->sender_MAC_address;

                                is_there_a_reciever = true;
                                layer_3->sender_MAC_address = clients[i].client_mac;
                                layer_3->receiver_MAC_address = betwwen_reciever_MAC;
                                stack<Packet *> new_packets;
                                new_packets.push(layer_0);
                                new_packets.push(layer_1);
                                new_packets.push(layer_2);
                                new_packets.push(layer_3);
                                clients[i].outgoing_queue.push(new_packets);
                                std::cout << "Frame #" << frame_index << " MAC address change: New sender MAC "
                                          << clients[i].client_mac << ", new receiver MAC "
                                          << betwwen_reciever_MAC << endl;
                                if (!layer_0->message_data.empty()) {
                                    if (layer_0->message_data.find('.') != std::string::npos ||
                                        layer_0->message_data.find('!') != std::string::npos ||
                                        layer_0->message_data.find('?') != std::string::npos) {

                                        int num_of_hops = 0;
                                        for (int j = 0; j < client_num; ++j) {
                                            if (clients[j].client_mac == sender_Mac) {
                                                if (!clients[j].log_entries.empty()) {
                                                    for (int k = 0; k < clients[j].log_entries.size(); ++k) {
                                                        if (clients[j].log_entries[k].message_content.find(
                                                                layer_0->message_data) != std::string::npos) {
                                                            num_of_hops = clients[j].log_entries[k].number_of_hops + 1;
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        time_t currentTime = time(nullptr);
                                        struct tm *timeinfo = localtime(&currentTime);
                                        char buffer[20];
                                        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);


                                        Log forwarded_log(buffer, message, frame_index, num_of_hops, layer_0->sender_ID,
                                                          layer_0->receiver_ID, true,
                                                          ActivityType(ActivityType::MESSAGE_FORWARDED));
                                        clients[i].log_entries.push_back(forwarded_log);
                                    }
                                }


                            } else {
                                int num_of_hops = 0;
                                for (int j = 0; j < client_num; ++j) {
                                    if (clients[j].client_mac == layer_3->sender_MAC_address) {
                                        if (!clients[j].log_entries.empty()) {
                                            for (int k = 0; k < clients[j].log_entries.size(); ++k) {
                                                if (!clients[j].log_entries[k].message_content.empty() &&
                                                    !layer_0->message_data.empty()) {
                                                    if (clients[j].log_entries[k].message_content.find(
                                                            layer_0->message_data) != std::string::npos) {
                                                        num_of_hops = clients[j].log_entries[k].number_of_hops + 1;
                                                    }
                                                }

                                            }
                                        }
                                    }
                                }
                                if (frame_index != 1) {
                                    std::cout << "Client " << clients[i].client_id << " receiving frame #"
                                              << frame_index
                                              << " from client "
                                              << before_id
                                              << ", but intended for client " << layer_0->receiver_ID
                                              << ". Forwarding... "
                                              << endl;
                                }
                                std::cout << "Error: Unreachable destination. Packets are dropped after " << num_of_hops
                                          << " hops!" << endl;
                                if (!layer_0->message_data.empty()) {
                                    if (layer_0->message_data.find('.') != std::string::npos ||
                                        layer_0->message_data.find('!') != std::string::npos ||
                                        layer_0->message_data.find('?') != std::string::npos) {
                                        time_t currentTime = time(nullptr);
                                        struct tm *timeinfo = localtime(&currentTime);
                                        char buffer[20];
                                        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                                        Log dropped_log(buffer, message, frame_index, num_of_hops, layer_0->sender_ID,
                                                        layer_0->receiver_ID, false,
                                                        ActivityType(ActivityType::MESSAGE_DROPPED));
                                        clients[i].log_entries.push_back(dropped_log);
                                    }
                                }


                            }


                        }
                        if (!layer_0->message_data.empty()) {
                            if (layer_0->message_data.find('.') != std::string::npos ||
                                layer_0->message_data.find('!') != std::string::npos ||
                                layer_0->message_data.find('?') != std::string::npos) {
                                if (message_comp) {

                                    int num_of_hops = 0;
                                    for (int j = 0; j < client_num; ++j) {
                                        if (clients[j].client_mac == layer_3->sender_MAC_address) {
                                            if (!clients[j].log_entries.empty()) {
                                                for (int k = 0; k < clients[j].log_entries.size(); ++k) {
                                                    if (!clients[j].log_entries[k].message_content.empty() &&
                                                        !layer_0->message_data.empty()) {
                                                        if (clients[j].log_entries[k].message_content.find(
                                                                layer_0->message_data) != std::string::npos) {
                                                            num_of_hops = clients[j].log_entries[k].number_of_hops + 1;
                                                        }
                                                    }

                                                }
                                            }
                                        }
                                    }

                                    std::cout << "Client " << clients[i].client_id << " received the message " << "\""
                                              << message << "\"" << " from client " << layer_0->sender_ID << "."
                                              << endl;
                                    time_t currentTime = time(nullptr);
                                    struct tm *timeinfo = localtime(&currentTime);
                                    char buffer[20];
                                    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

                                    if (layer_0->receiver_ID == clients[i].client_id) {
                                        Log recieved_log(buffer, message, frame_index, num_of_hops, layer_0->sender_ID,
                                                         clients[i].client_id, true,
                                                         ActivityType(ActivityType::MESSAGE_RECEIVED));
                                        clients[i].log_entries.push_back(recieved_log);
                                    }

                                }
                                frame_index = 1;
                                message.clear();
                                std::cout << "--------" << endl;
                            } else {
                                frame_index += 1;
                            }
                        }

                        clients[i].incoming_queue.pop();
                    }
                }

            }
        } else if (command_words[0] == "PRINT_LOG") {
            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;

            for (int i = 0; i < client_num; ++i) {
                if (command_words[1] == clients[i].client_id) {
                    if (!clients[i].log_entries.empty()) {
                        std::cout << "Client " << clients[i].client_id << " Logs:" << endl;
                        for (int j = 0; j < clients[i].log_entries.size(); ++j) {
                            std::cout << "--------------" << endl;
                            std::cout << "Log Entry #" << j + 1 << ":" << endl;
                            std::cout << "Activity: " << clients[i].log_entries[j].activityTypeToString() << endl;
                            std::cout << "Timestamp: " << clients[i].log_entries[j].timestamp << endl;
                            std::cout << "Number of frames: " << clients[i].log_entries[j].number_of_frames << endl;
                            std::cout << "Number of hops: " << clients[i].log_entries[j].number_of_hops << endl;
                            std::cout << "Sender ID: " << clients[i].log_entries[j].sender_id << endl;
                            std::cout << "Receiver ID: " << clients[i].log_entries[j].receiver_id << endl;
                            std::cout << "Success: " << (clients[i].log_entries[j].success_status ? "Yes" : "No")
                                      << endl;
                            if (clients[i].log_entries[j].activityTypeToString() == "Message Received" ||
                                clients[i].log_entries[j].activityTypeToString() == "Message Sent") {
                                std::cout << "Message: " << "\"" << clients[i].log_entries[j].message_content << "\""
                                          << endl;
                            }

                        }
                    }

                }

            }

        } else {
            std::cout << dashes << endl;
            std::cout << "Command: " << s << endl;
            std::cout << dashes << endl;
            std::cout << "Invalid command." << endl;
        }
    }



// TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
/* Don't use any static variables, assume this method will be called over and over during testing.
 Don't forget to update the necessary member variables after processing each command. For example,
 after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::string line;
    std::getline(file, line);
    client_num = std::stoi(line);
    string id;
    string ip;
    string mac;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        while (iss >> id >> ip >> mac) {
            clients.emplace_back(id, ip, mac);
        }

        // TODO: Read clients from the given input file and return a vector of Client instances.
    }

    file.close();

    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    ifstream routing_file(filename);
    string line;
    int client_index = 0;
    while (std::getline(routing_file, line)) {
        std::istringstream iss(line);
        string first;
        string second;
        while (iss >> first >> second) {
            clients[client_index].routing_table.insert(make_pair(first, second));
        }

        if (!line.empty()) {
            if (line.find('-') != std::string::npos) {
                client_index++;
                continue;
            }
        }


    }

    routing_file.close();
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::string line;
    std::getline(file, line);
    command_num = std::stoi(line);
    while (std::getline(file, line)) {
        commands.push_back(line);
    }

    file.close();
    // TODO: Read commands from the given input file and return them as a vector of strings.
    return commands;
}

Network::~Network() {
    for (int i = 0; i < vector_stack.size(); ++i) {
        for (int k = 0; k < 4; ++k) {
            delete vector_stack[i].top();
            vector_stack[i].pop();
        }
    }
    // TODO: Free any dynamically allocated memory if necessary.
}
