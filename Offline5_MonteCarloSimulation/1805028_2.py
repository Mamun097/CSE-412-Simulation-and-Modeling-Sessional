import os
import sys
import random
import numpy as np
import matplotlib.pyplot as plt

class InterviewSimulation:
    def __init__(self, n, s, m, iterations=10000):
        self.n = n
        self.s = s
        self.m = m
        self.iterations = iterations

    def run_simulation(self):
        success_count = 0
        for iteration in range(self.iterations):
            n_list = [random.uniform(0,1) for _ in range(self.n)]
            m_index = random.sample(list(range(self.n)), self.m)
            m_list = [n_list[i] for i in m_index]

            criteria = np.max(m_list) if m_list else 0
            selected_index = self.interview(n_list, m_index, criteria)
            if self.check_success(n_list, selected_index):
                success_count += 1
        return success_count / self.iterations

    def interview(self, n_list, m_index, criteria):
        for i in range(self.n):
            if i not in m_index and n_list[i] > criteria:
                return i
        return self.n - 1

    def check_success(self, n_list, selected_index):
        sorted_n_list = np.argsort(n_list)
        sorted_n = np.array(n_list)[sorted_n_list]
        top_scorers = sorted_n[-self.s:]
        return n_list[selected_index] in top_scorers

if __name__ == '__main__':
    n = int(sys.argv[1])
    select = [1, 3, 5, 10]
    output_dir = "graphs_problem2"
    os.makedirs(output_dir, exist_ok=True)

    for s in select:
        success_rate_list = []
        for m in range(n):
            simulation = InterviewSimulation(n, s, m)
            success_rate = simulation.run_simulation()
            success_rate_list.append(success_rate * 100)

        plt.figure(figsize=(6, 6))
        plt.title('Success Rate vs. m\ns = ' + str(s) + ', n = ' + str(n) + ', 10000 iterations')
        plt.plot(range(n), success_rate_list)
        plt.xlabel('m')
        plt.ylabel('Success Rate')
        plt.savefig(f"{output_dir}/graph_s={s}.png")
        plt.close()
        print(f"Graph for s={s} is saved in {output_dir}")