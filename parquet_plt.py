import pandas as pd
import matplotlib.pyplot as plt

def cpp_plt(filename):
    df = pd.read_parquet(filename, engine="pyarrow")
    plt.figure(figsize=(10, 6))
   
    for column in df.columns:
        data = pd.to_numeric(df[column].dropna())
        plt.plot(data, label=column, marker='o', linestyle='none', markersize=2)

    plt.title("CPP_EXPORT")
    plt.xlabel("Index")
    plt.ylabel("Value")
    plt.yscale('log')
    plt.legend()
    plt.grid(True, which="both", ls="-", alpha=0.5)
    plt.show()

if __name__ == "__main__":
    cpp_plt("export.parquet")
