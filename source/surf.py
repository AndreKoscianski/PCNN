from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import matplotlib
import pandas as pd
import seaborn as sns
import numpy
import sys

try:
    vals = numpy.genfromtxt(sys.argv[1], delimiter=',')
except:
    print ('não consegui abrir arquivo')
    sys.exit (-1)

#valp = numpy.genfromtxt('fp.csv', delimiter=',')

val_xdim, val_ydim = vals.shape


print (vals.shape)

xax = numpy.arange(0, val_xdim)
yax = numpy.arange(0, val_ydim)
mx, my = numpy.meshgrid(yax, xax)


fig, ax = plt.subplots(subplot_kw={"projection": "3d"})
ax.plot_surface(mx, my, vals, cmap=matplotlib.cm.Blues)

#ax.set(xticklabels=[],
#       yticklabels=[],
#       zticklabels=[])

plt.show()


#fig = plt.figure()
#ax = fig.gca()#projection='3d')
#surf = ax.plot_surface(xax, yax, vals, rstride=1, cstride=1, cmap='viridis', linewidth=0, antialiased=False)
#ax.plot_wireframe(xax, yax, vals, color='k', lw=0.05, alpha=0.3)
#fig.colorbar(surf, shrink=0.5, aspect=5)

