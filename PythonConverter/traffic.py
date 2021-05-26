import numpy as np
import pandas
import matplotlib as mpl
import matplotlib.pyplot as plt
from sympy import *

if __name__ == '__main__':
    data = pandas.read_csv('output_in3.csv')

    func = data.iloc[:, 0]
    GW_1 = data.iloc[:, 1]
    GW_2 = data.iloc[:, 2]
    BW_1 = data.iloc[:, 3]
    BW_2 = data.iloc[:, 4]
    M1 = data.iloc[:, 5]
    M2 = data.iloc[:, 6]
    load = data.iloc[:, 7]

    i = 0
    ii = 0
    while (func[i] < 0):
        i += 1
        ii += 1
    # i += 2
    # ii += 2

    # Зафиксируем длительность зеленых сигналов светофора при плохой походе
    fix_BW_1 = BW_1[i]
    fix_BW_2 = BW_2[i]

    size = sqrt(sqrt(len(func))) * 3
    print("size = " + str(size))

    F = np.zeros((size, size))
    L = np.zeros((size, size))
    F[:, :] = None
    T1 = np.zeros(size)
    T2 = np.zeros(size)

    # Начальное значение
    # fv = 20
    fv = 15

    for i in range(0, len(func)):
        if (BW_1[i] == fix_BW_1 and BW_2[i] == fix_BW_2):
            if (func[i] > -1):
                for j in range(0, 3):
                    F[GW_1[i] - fv + j][GW_2[i] - fv + j] = func[i]
                    T1[GW_1[i] - fv + j] = GW_1[i]
                    T2[GW_2[i] - fv + j] = GW_2[i]

    for i in range(0, len(func)):
        L[GW_2[i] - fv][GW_1[i] - fv] = load[i]

    vmin, vmax = 15, 40
    norm = mpl.colors.Normalize(vmin=vmin, vmax=vmax)

    plt.figure()
    plt.pcolor(T1, T2, F, norm=norm, cmap=plt.cm.get_cmap('rainbow_r'))
    plt.colorbar()

    plt.xlim(15, 60)
    plt.ylim(15, 60)

    x = np.linspace(15, 60, 46)
    y = (0.075 / (0.065) * (x / 4).round()).round() * 4
    plt.plot(x, y, color="black", lw=10)

    # for T1 in range(0,size-1):
    #     for T2 in range(0,size-1):
    #         plt.text(T1 + fv + 0.5, T2 + fv + 0.5, '%.3f' % L[T1][T2],
    #                  horizontalalignment='center',
    #                  verticalalignment='center',
    #                  )

    plt.xlabel("Время зеленого света для второго потока - хорошая погода (GW)")
    plt.ylabel("Время зеленого света для первого потока - хорошая погода (GW)")
    plt.title(
        'Значение среднего времени ожидания при фиксированных значениях времени зеленего света при плохой погоде (BW_1 = %d, BW_2 = %d)' % (
            fix_BW_1, fix_BW_2))

    # Зафиксируем время при хорошей походе
    fix_GW_1 = GW_1[ii]
    fix_GW_2 = GW_2[ii]

    F_ = np.zeros((size, size))
    L_ = np.zeros((size, size))
    F_[:, :] = None
    T1_ = np.zeros(size)
    T2_ = np.zeros(size)

    for i in range(0, len(func)):
        if (GW_1[i] == fix_GW_1 and GW_2[i] == fix_GW_2):
            if (func[i] > -1):
                for j in range(0, 3):
                    F_[BW_1[i] + j - fv][BW_2[i] + j - fv] = func[i]
                    T1_[BW_1[i] + j - fv] = BW_1[i]
                    T2_[BW_2[i] + j - fv] = BW_2[i]

    for i in range(0, len(func)):
        L_[BW_2[i] - fv][BW_1[i] - fv] = load[i]

    plt.figure()
    plt.pcolormesh(T1_, T2_, F_, cmap='rainbow_r')
    plt.colorbar()

    plt.xlim(15, 60)
    plt.ylim(15, 60)

    x1 = np.linspace(15, 60, 45)
    y1 = ((0.045 * 1.45 / (0.055 * 1.6) * (x1 / 4)).round()).round() * 4
    plt.plot(x1, y1, color="black", lw=10)

    # for T1_ in range(0, size - 1):
    #     for T2_ in range(0, size - 1):
    #         plt.text(T1_ + fv + 0.5, T2_ + fv + 0.5, '%.3f' % L_[T1_][T2_],
    #                  horizontalalignment='center',
    #                  verticalalignment='center',
    #                  )

    plt.xlabel("Время зеленого света для второго потока - плохая погода (BW)")
    plt.ylabel("Время зеленого света для первого потока - плохая погода (BW)")
    plt.title(
        'Значение среднего времени ожидания при фиксированных значениях времени зеленего света при хорошей погоде (GW_1 = %d, GW_2 = %d)' % (
            fix_GW_1, fix_GW_2))
    plt.show()
