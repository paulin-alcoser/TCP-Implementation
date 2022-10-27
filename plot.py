import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("cwnd.csv", header = None)

dict = {
    0: 'Time',
    1: 'CWND'
}
 
# call rename () method
df.rename(columns=dict,
          inplace=True)

plt.rcParams.update({'font.size': 30})
plot = df.plot(x="Time", y="CWND", figsize = (50,20), xlabel="Time", ylabel="CWND", 
                fontsize = 25, title ="Time vs CWND" )

plot.get_figure().savefig('CWND.pdf', format='pdf')

plt.show()