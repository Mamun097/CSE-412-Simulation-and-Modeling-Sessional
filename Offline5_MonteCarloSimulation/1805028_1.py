import random
import numpy as np

class MonteCarloSimulation:
    def __init__(self, n, p0, pi, num_iteration, output_file):
        self.n = n
        self.p0 = p0
        self.pi = pi
        self.num_iteration = num_iteration
        self.output_file = output_file
        self.probability = np.zeros(5)

    def run_simulation(self):
        counters = [0, 0, 0, 0, 0]
        p1 = self.p0 + self.pi[0]
        p2 = p1 + self.pi[1]
        p3 = p2 + self.pi[2]

        for i in range(self.num_iteration):
            num_neutrons = self.simulate_generation(p0, p1, p2, p3)
            counters[num_neutrons if num_neutrons < 4 else 4] += 1

        for i in range(5):
            self.probability[i] = counters[i] / self.num_iteration

    def simulate_generation(self, p0, p1, p2, p3):
        num_neutrons = 1
        for _ in range(self.n):
            new_neutrons = 0
            for _ in range(num_neutrons):
                r = random.uniform(0, 1)
                new_neutrons += self.get_neutron_count(r, p0, p1, p2, p3)
            num_neutrons = new_neutrons
        return num_neutrons

    def get_neutron_count(self, r, p0, p1, p2, p3):
        if r < p0:
            return 0
        elif r < p1:
            return 1
        elif r < p2:
            return 2
        elif r < p3:
            return 3
        else:
            return 4

    def write_output(self):
        with open(self.output_file, "a") as file:
            file.write(f"Generation-{self.n}\n")
            for i in range(5):
                file.write(f"p[{i}] = {self.probability[i]}\n")
            file.write("\n")

if __name__ == '__main__':
    n = 10
    p0 = 0.4825
    pi = [(0.2126) * (0.5893) ** (i-1) for i in range(1, 4)]
    prob_sum = sum(pi) + p0

    if prob_sum != 1:
        p0 = p0 + (1 - prob_sum)

    num_iteration = 10000
    output_file = "/home/m/Desktop/CSE-412-Simulation-and-Modeling-Sessional/Offline5_MonteCarloSimulation/output_problem1.txt"
    open(output_file, 'w').close()

    for i in range(1, n+1):
        simulation = MonteCarloSimulation(i, p0, pi, num_iteration, output_file)
        simulation.run_simulation()
        simulation.write_output()