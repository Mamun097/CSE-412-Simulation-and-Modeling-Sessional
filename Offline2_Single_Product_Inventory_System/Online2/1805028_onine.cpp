#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include "lcgrand.h"

void initialize();
void timing();
void order_arrival();
void update_time_avg_stats();
void evaluate();
void report();
int random_integer(float prob_distrib[]);
float uniform(float a, float b);
float expon(float mean);
void demand();
void express_order_arrival(float E_K, float E_i, float E_m, float E_M);

int amnt, bigs, number_of__events, initial_inventory_level, inventory_level, next_event_type,
    number_of_months, num_values_demand, smalls;
float mean_interdemand, minlag, prob_distrib_demand[26], area_holding, area_shortage, holding_cost, incremental_cost, maxlag,
     setup_cost, shortage_cost, simulation_time, time_last_event, time_next_event[5],
    total_ordering_cost, E_K, E_i, E_m, E_M;

std::ifstream input_file;
std::ofstream output_file;

void initialize() {
    simulation_time = 0.0;

    total_ordering_cost = 0.0;

    inventory_level = initial_inventory_level;
    time_last_event = 0.0;
    area_holding = 0.0;
    area_shortage = 0.0;

    // Express order parameters
    E_K = 0.0;
    E_i = 0.0;
    E_m = 0.0;
    E_M = 0.0;

    time_next_event[1] = 1.0e+30;
    time_next_event[2] = simulation_time + expon(mean_interdemand);
    time_next_event[3] = number_of_months;
    time_next_event[4] = 0.0;
}

void order_arrival() {
    inventory_level += amnt;
    time_next_event[1] = 1.0e+30;
}

void demand() {
    inventory_level -= random_integer(prob_distrib_demand);
    time_next_event[2] = simulation_time + expon(mean_interdemand);
}

void express_order_arrival(float E_K, float E_i, float E_m, float E_M) {
    //amnt = bigs - inventory_level;
    inventory_level += amnt;
    total_ordering_cost += E_K + E_i * amnt;

    time_next_event[1] = simulation_time + uniform(E_m, E_M);
}

void evaluate() {
    if (inventory_level < 0) {
        // If inventory is negative, place express order
        express_order_arrival(E_K, E_i, E_m, E_M);
    }

    else if (inventory_level < smalls) {
        amnt = bigs - inventory_level;
        total_ordering_cost += setup_cost + incremental_cost * amnt;

        time_next_event[1] = simulation_time + uniform(minlag, maxlag);
    }

    time_next_event[4] = simulation_time + 1.0;
}

void report() {
    float avg_holding_cost, avg_ordering_cost, avg_shortage_cost;
    avg_holding_cost = holding_cost * area_holding / number_of_months;
    avg_ordering_cost = total_ordering_cost / number_of_months;
    avg_shortage_cost = shortage_cost * area_shortage / number_of_months;
    output_file << "\n\n(" << std::setw(3) << smalls << "," << std::setw(3) << bigs << ")"
            << std::setw(18) << avg_ordering_cost + avg_holding_cost + avg_shortage_cost
            << std::setw(15) << avg_ordering_cost << std::setw(24) << avg_holding_cost
            << std::setw(18) << avg_shortage_cost;
}

void update_time_avg_stats() {
    float time_since_last_event;

    time_since_last_event = simulation_time - time_last_event;
    time_last_event = simulation_time;

    if (inventory_level > 0)
        area_holding += inventory_level * time_since_last_event;
    else if (inventory_level < 0)
        area_shortage -= inventory_level * time_since_last_event;
}

int random_integer(float prob_distrib[]) {
    int i;
    float u;

    u = lcgrand(1);

    for (i = 1; u >= prob_distrib[i]; ++i)
        ;

    return i;
}

float uniform(float a, float b) {
    return a + lcgrand(1) * (b - a);
}

void timing() {
    int i;
    float min_time_next_event = 1.0e+29;
    next_event_type = 0;

    for (i = 1; i <= number_of__events; ++i)
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type = i;
        }

    if (next_event_type == 0) {
        output_file << "\nEvent list empty at time " << simulation_time;
        exit(1);
    }

    simulation_time = min_time_next_event;
}

float expon(float mean) {
    return -mean * log(lcgrand(1));
}

int main() {
    int i, number_of_policies, T;

    input_file.open("in.txt");
    output_file.open("out.txt");

    number_of__events = 5; // Updated for express order event

    // Read the number of trials (T) from the input file
    input_file >> T;

    // Read other input parameters for each trial
    input_file >> initial_inventory_level >> number_of_months >> number_of_policies >> num_values_demand
            >> mean_interdemand >> setup_cost >> incremental_cost >> holding_cost
            >> shortage_cost >> minlag >> maxlag;

    // Express order parameters
    input_file >> E_K >> E_i >> E_m >> E_M;

    // ...

    // for (int trial = 0; trial < T; ++trial) {

        // Loop over policies
    for (i = 1; i <= number_of_policies; ++i) {
        input_file >> smalls >> bigs;
        initialize();

        do {
            timing();
            update_time_avg_stats();

            if (next_event_type == 1)
                order_arrival();
            else if (next_event_type == 2)
                demand();
            else if (next_event_type == 4)
                evaluate();
            else if (next_event_type == 3)
                report();
            else if (next_event_type == 5) // Express order event
                express_order_arrival(E_K, E_i, E_m, E_M);
        } while (next_event_type != 3);
    }
    

    output_file << "\n\n------------------------------------------------------------------------------------------------";
    input_file.close();
    output_file.close();
    return 0;
}
