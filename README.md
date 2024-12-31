# PCNN
Physically Constrained Neural Network

Implementation of Single-Layer Perceptron, Multi-Layer Perceptron, and Radial-Basis Network, all of them with the capability to be trained under (physical) restrictions.

For instance: given a neural network y=f(x,w) with parameters w, it is possible to "train" it to minimize df/dx(x) = x , meaning that the function your network will reproduce is y=x^2 + C.

The "training" is not other thing than a minimization. For that purpose the package provides a sort of Particle Swarm Search with an exponential decay that, in the tests, performed very well.
