#!/usr/bin/env python

from numpy import *
from numpy import linalg
import numpy as np
import matplotlib.pyplot as plt

# http://scipy.github.io/old-wiki/pages/Numpy_Example_List
# http://docs.scipy.org/doc/numpy/reference/generated/numpy.cross.html
# http://docs.scipy.org/doc/scipy/reference/tutorial/linalg.html
# http://mathworld.wolfram.com/RotationMatrix.html


def test_stuff():
    ##############################
    # normalize a matrix
    print("normalize a matrix")
    M = [[ 1, 2, 3],
         [-2, 1, 4],
         [-3,-4, 1]]
    M  = matrix(M)
    print(M)
    Mn = linalg.norm(M)
    print(Mn)
    M = M/Mn
    print(M)
    print()
    ##############################
    # normalize a vector
    print("normalize a vector")
    A  = [1, 2, 3]
    A  = array(A)
    print(A)
    Anorm = linalg.norm(A)
    print(Anorm)
    B = A/Anorm
    print(B)

    ##############################

    def normalized(a, axis=-1, order=2):
        l2 = atleast_1d(linalg.norm(a, order, axis))
        l2[l2==0] = 1
        return a / expand_dims(l2, axis)

    #A = random.randn(3,3,3)
    A = random.randn(3,3)
    print(A)
    print("now normalize row 0 of A")
    print(normalized(A,0))
    #print normalized(A,1)
    #print normalized(A,2)

    #print normalized(arange(3)[:,None])
    #print normalized(arange(3))
    
    ##############################

    # enter - gyro readings for roll, pitch and yaw

    print("Premerlani DCM algorithm")
    
    # Body-to-Earth Rotation Matrix:
    Reb = [[ 1, 2, 3],
           [-2, 1, 4],
           [-3,-4, 1]]

    # Earth-to-Body (Inverse) Rotation Matrix:
    Reb  = matrix(Reb)
    Rbe  = linalg.inv(Reb)
    Rbet = matrix.transpose(Reb)

    print(Reb)
    print(Rbe)
    print(Rbet)

    bx = 1
    by = 2
    bz = 3

    Bb  = [bx, by, bz]
    Bb  = matrix(Bb)
    Bbt = matrix.transpose(Bb)

    print(Reb)
    print(Bb)
    print(Bbt)




T = [[1,2,47,11],[3,2,8,15],[0,0,3,1],[0,0,8,1]]
S = [[113,49,2,283],[-113,0,3,359],[0,5,0,6],[0,20,0,12]]

T = matrix(T)
S = matrix(S)
print(S * T); print()


A = matrix('1 3 4; 5 6 9; 7 9 2') # matrix is synonymous with mat
B = matrix([[1, 3, 4],
            [5, 6, 9],
            [1, 2, 3]])
C = linalg.inv(A)
print("S^{-1} * A * B")
print(linalg.inv(B) * A * B); print

F = matrix([[0.1, 0.3, 0.4],
            [0.5, 0.6, 0.9],
            [0.1, 0.2, 0.3]])

G = matrix([[0.0, 0.0, 1.0],
            [0.0, 0.0, 1.0],
            [0.0, 0.0, 1.0]])

print(F * matrix.transpose(G)); print

print("determinant")
print(linalg.det(F)); print


# rotation 
print("2D rotate")
theta = 1.2 #radian
st = sin(theta)
ct = cos(theta)
Rth = [[ct, -st], [st, ct]]
Rth = matrix(Rth)
print(Rth); print

print("3D rotate")
theta = 1.2 #radian
a = 2.3 # alpha
b = 3.5 # beta
g = 4.5 # gamma
sa = sin(a)
ca = cos(a)
sb = sin(b)
cb = cos(b)
sg = sin(g)
cg = cos(g)
Rx = [[ 1,  0,   0], [  0, ca, sa], [ 0, -sa, ca]]
Ry = [[cb,  0, -sb], [  0,  1,  0], [sb,   0, cb]]
Rz = [[cg, sg,   0], [-sg, cg,  0], [ 0,   0,  1]]
Rx = matrix(Rx)
Ry = matrix(Ry)
Rz = matrix(Rz)
Rot = Rx * Ry * Rz
print(Rx); print
print(Ry); print
print(Rz); print
print(Rot); print

# DCM - angles in degrees
print("DCM")
degrees_to_radians = 0.0174533
theta = 10 * degrees_to_radians
gamma = 20 * degrees_to_radians
psi   = 30 * degrees_to_radians

st = sin(theta)
ct = cos(theta)
sg = sin(gamma)
cg = cos(gamma)
sp = sin(psi)
cp = cos(psi)

rxx = ct*cg
ryx = ct*sg
rzx = -st
rxy = (sp*st*cg) - (cp*sg)
ryy = (sp*st*sg) + (cp*cg)
rzy = sp*ct
rxz = (cp*st*cg) + (sp*sg)
ryz = (cp*st*sg) - (sp*cg)
rzz = cp*ct

R = [[ rxx, ryx, rzx],
     [ rxy, ryy, rzy],
     [ rxz, ryz, rzz]]
R = matrix(R)

print("DCM Identity - via transpose")
I = transpose(R) * R
print(I); print
print("DCM Identity - via inverse")
I = linalg.inv(R) * R
print(I); print

Rx = [rxx, rxy, rxz]
Ry = [ryx, ryy, ryz]
Rz = [rzx, rzy, rzz]
Rx = matrix(Rx)
Ry = matrix(Ry)
Rz = matrix(Rz)

print("Rz is cross(Rx, Ry)")
Rz = cross(Rx, Ry)
print(Rz); print
print("these are the errors:")
print(transpose(Rx)*Ry); print
print(transpose(Rx)*Rz); print
print(transpose(Rz)*Ry); print

# re-normalization
print("normalize")
Xn = 0.5 * (3 - transpose(Rx) * Rx) * transpose(Rx)
Yn = 0.5 * (3 - transpose(Ry) * Ry) * transpose(Ry)
Zn = 0.5 * (3 - transpose(Rz) * Rz) * transpose(Rz)
print(Rx)
print(Ry)
print(Rz)

# chess board
Rook = [[0,0,0,0,0,0,1,0],
        [0,0,0,0,0,0,0,1],
        [0,0,0,0,0,1,0,0],
        [1,0,0,0,0,0,0,0],
        [0,0,0,0,1,0,0,0],
        [0,0,1,0,0,0,0,0],
        [0,0,0,1,0,0,0,0],
        [0,1,0,0,0,0,0,0]]
Rook  = matrix(Rook)
Rook_inv = None
print(Rook)
print(linalg.det(Rook))
if (linalg.det(Rook) > 0.0):
    Rook_inv = linalg.inv(Rook)
    print(Rook_inv)
    print()
    print(Rook_inv*Rook)
else:
    print("Rook is singular")

#plt.figure(1)
#plt.plot(xx, y.T, color='r')
#plt.show()
#plt.matshow(np.random.random((50,50)));

#plt.matshow(Rook);
#plt.matshow(Rook_inv);
#plt.colorbar()
#plt.show()

GR     = [[1,2,3],
          [3,1,2],
          [1,1,1]]
GR_inv = linalg.inv(GR)
plt.matshow(GR);
plt.matshow(GR_inv);
plt.colorbar()
plt.show()


