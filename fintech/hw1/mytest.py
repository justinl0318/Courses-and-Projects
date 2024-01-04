import pandas as pd
import numpy as np

def custom_rolling_mean(nparr, window):
    arr = nparr.copy()
    for i in range(1, len(arr)):
        start = max(0, i - window + 1)
        end = i + 1
        arr[i] = nparr[start:end].mean()
    return pd.Series(arr)

window_size = [5, 10, 20, 60]
price_diff = df["Adj Close"].diff(1)
positive_price_changes = price_diff.where(price_diff > 0, 0)
negative_price_changes = -price_diff.where(price_diff < 0, 0)

for window in window_size:
    #moving_average = df["Adj Close"].rolling(window).mean()
    moving_average = custom_rolling_mean(df["Adj Close"].values, window)
    name = "MA_ratio_{}".format(window)
    myname = "My_MA_r#atio_{}".format(window)
    df[name] = df["Adj Close"] / moving_average
    features.append(name)

    average_gain = custom_rolling_mean(positive_price_changes.values, window)
    average_loss = custom_rolling_mean(negative_price_changes, window)
    relative_strength = average_gain / average_loss
    rsi = 100 - (100 / (1 + relative_strength))
    name = "RSI_window_{}".format(window)
    df[name] = rsi
    features.append(name)
    