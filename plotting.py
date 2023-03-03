"""
File: plotting.py
----------------
I couldn't figure out how to install matplotlib-cpp so we're using Python
"""
import matplotlib.pyplot as plt
import pandas as pd


def plot_experiment_1():
    data = pd.read_csv("output/experiment_1.csv").iloc[:, 0].values
    min_utility = min(data)
    max_utility = max(data)
    plt.hist(data, bins=range(min_utility, max_utility + 1, 50))
    plt.xlabel("Average Utility Increase")
    plt.ylabel("Frequency")
    plt.title("Agent Utility Increases from Ex-Post Pairwise Transfers")
    plt.savefig("output/experiment_1.png")


def plot_experiment_3():
    data = pd.read_csv("output/experiment_3.csv")
    indices = data.iloc[:, 0].values
    utility = data.iloc[:, 1].values
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.plot(indices, utility)
    ax.set_xscale("log")
    plt.xlabel("Friction as a Proportion of Transaction Cost")
    plt.ylabel("Percentage Increase over RSD Baseline")
    plt.title("Utility Increases from Ex-Post Pairwise Transfers with Varying Friction")
    plt.savefig("output/experiment_3.png")


def main():
    plot_experiment_1()
    plot_experiment_3()


if __name__ == "__main__":
    main()
