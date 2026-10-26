import sys

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

path = sys.argv[1] if len(sys.argv) > 1 else "results_summary.csv"
df = pd.read_csv(path)

scenarios = {
    "asc_insert": "Inserção Ascendente",
    "desc_insert": "Inserção Descendente",
    "rand_insert": "Inserção Aleatória",
    "rand_remove": "Remoção Aleatória",
}
trees = {"AVL": ("AVL", "o"), "RB": ("Rubro-Negra", "s")}


def theoretical_time(ax, data):
    ops = np.linspace(data["ops"].min(), data["ops"].max(), num=350)
    max_ops = data["ops"].max()
    adjust_factor = data["time_ms_mean"].max() / (max_ops * np.log2(max_ops))
    ax.plot(
        ops,
        ops * np.log2(ops) * adjust_factor,
        label=r"$O(n \log n)$ teórico",
        linestyle="--",
        color="gray",
        alpha=0.5,
    )


def theoretical_height(ax, data):
    n = np.linspace(data["ops"].min(), data["ops"].max(), num=350)
    ax.plot(
        n,
        1.44 * np.log2(n + 2),
        label=r"Limite AVL ($1{,}44 \log_2 n$)",
        linestyle="--",
        color="gray",
        alpha=0.5,
    )
    ax.plot(
        n,
        2 * np.log2(n + 1),
        label=r"Limite RB ($2 \log_2 n$)",
        linestyle=":",
        color="gray",
        alpha=0.8,
    )


def theoretical_rotations(ax, data):
    ops = np.linspace(data["ops"].min(), data["ops"].max(), num=350)
    adjust_factor = data["rotations_mean"].max() / data["ops"].max()
    ax.plot(
        ops,
        ops * adjust_factor,
        label=r"$O(n)$ teórico",
        linestyle="--",
        color="gray",
        alpha=0.5,
    )


def plot_metric(column, ylabel, filename, theoretical):
    fig, axes = plt.subplots(2, 2, figsize=(11, 8))

    for ax, (scenario, title) in zip(axes.flat, scenarios.items()):
        data = df[df["scenario"] == scenario]

        for tree, (label, marker) in trees.items():
            tree_data = data[data["tree"] == tree]
            ax.plot(tree_data["ops"], tree_data[column], label=label, marker=marker)

        theoretical(ax, data)

        ax.set(title=title, xlabel="Número de Operações (n)", ylabel=ylabel)
        ax.grid()

    handles, labels = axes.flat[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncol=len(labels))
    fig.tight_layout(rect=(0, 0.06, 1, 1))
    fig.savefig(filename, bbox_inches="tight")
    plt.close(fig)


plot_metric(
    "time_ms_mean", "Tempo de Execução (ms)", "grafico_tempos.pdf", theoretical_time
)
plot_metric(
    "height_mean", "Altura da Árvore", "grafico_alturas.pdf", theoretical_height
)
plot_metric(
    "rotations_mean",
    "Número de Rotações",
    "grafico_rotacoes.pdf",
    theoretical_rotations,
)
