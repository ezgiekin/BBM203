#include "Leaderboard.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

void Leaderboard::insert_new_entry(LeaderboardEntry *new_entry) {
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry *cur = head_leaderboard_entry;
        LeaderboardEntry *previous = nullptr;

        while (cur && new_entry->score <= cur->score) {
            previous = cur;
            cur = cur->next_leaderboard_entry;
        }

        previous->next_leaderboard_entry = new_entry;
        new_entry->next_leaderboard_entry = cur;
    }
    size++;

    if (size > 10) {
        while(size > 10){
            if (!head_leaderboard_entry || !head_leaderboard_entry->next_leaderboard_entry) {
                return;
            }
            LeaderboardEntry *current = head_leaderboard_entry;
            LeaderboardEntry *prev = nullptr;
            while (current->next_leaderboard_entry) {
                prev = current;
                current = current->next_leaderboard_entry;
            }
            prev->next_leaderboard_entry = nullptr;
            delete current;
            size--;
        }

    }

    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
}

void Leaderboard::write_to_file(const string &filename) {
    std::ofstream file(filename, std::ios::trunc);

    if (!file.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
    }

    LeaderboardEntry *o;
    o = head_leaderboard_entry;
    while (o) {
        file << o->score << " " << o->last_played << " " << o->player_name << std::endl;
        o = o->next_leaderboard_entry;
        // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    }

    file.close();



    // TODO: Write the latest leaderboard status to the given file in the format specified in the PA instructions
}

void Leaderboard::read_from_file(const string &filename) {
    std::ifstream leader_file(filename);

    if (leader_file.is_open()) {
        string person;
        while (getline(leader_file, person)) {
            if (!person.empty() && size != 10) {
                istringstream iss(person);
                std::vector<std::string> tokens;
                std::string token;

                while (std::getline(iss, token, ' ')) {
                    tokens.push_back(token);
                }

                LeaderboardEntry *leaderboardEntry = new LeaderboardEntry(std::stoll(tokens[0]), std::stoll(tokens[1]),
                                                                          tokens[2]);
                if (!head_leaderboard_entry) {
                    head_leaderboard_entry = leaderboardEntry;
                    head_leaderboard_entry->next_leaderboard_entry = nullptr;
                } else {
                    LeaderboardEntry *p;
                    p = head_leaderboard_entry;
                    while (p->next_leaderboard_entry != nullptr) {
                        p = p->next_leaderboard_entry;
                    }
                    p->next_leaderboard_entry = leaderboardEntry;
                    leaderboardEntry->next_leaderboard_entry = nullptr;
                }
                size++;
            }
        }

        leader_file.close();
    }



}

// TODO: Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
//       variable will point to the highest all-times score, and all other scores will be reachable from it
//       via the "next_leaderboard_entry" member variable pointer.



void Leaderboard::print_leaderboard() {
    LeaderboardEntry *o;
    int i = 1;
    o = head_leaderboard_entry;
    std::cout << "Leaderboard\n-----------" << std::endl;
    while (o) {
        struct tm *timeinfo = localtime(&o->last_played);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", timeinfo);
        std::cout << i << ". " << o->player_name << " " << o->score << " " << buffer << std::endl;
        o = o->next_leaderboard_entry;
        i++;
        // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

Leaderboard::~Leaderboard() {

    LeaderboardEntry *current = head_leaderboard_entry;
    LeaderboardEntry *next_leaderboardEntry;
    while (current) {
        next_leaderboardEntry = current->next_leaderboard_entry;
        delete current;
        current = next_leaderboardEntry;
    }
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
}
