import numpy as np
#import tensorflow as tf
import tensorflow.compat.v1 as tf
import matplotlib.pyplot as plt 

tf.disable_v2_behavior()

# https://medium.com/datathings/a-neural-network-fully-coded-in-numpy-and-tensorflow-cc275c2b14dd
# https://github.com/assaad/neuralnet_example/blob/master/NN_in_pure_TF.ipynb

#config
inputSize = 5
outputSize = 3
batchSize = 10
testSize = 5
epochs = 400
learningRate = 0.01
seed = 123456


#Generation of Dataset
np.random.seed(seed)
inputArray = np.random.uniform(-5,5,(batchSize,inputSize))
weights = np.random.uniform(-5,5,(inputSize,outputSize))
outputArray = np.matmul(inputArray,weights)
inputTest = np.random.uniform(-5,5,(testSize,inputSize))
outputTest = np.matmul(inputTest,weights)


inputArray
outputArray
inputTest
outputTest

#initialization of NN by other random weights
nnWeights = np.random.uniform(-3,3,(inputSize,outputSize))
deltaweights = np.zeros((inputSize,outputSize))
deltainput = np.zeros((batchSize,inputSize))
deltaoutput = np.zeros((inputSize,outputSize))

weights
nnWeights

inputArray_tf = tf.convert_to_tensor(inputArray)
weights_tf = tf.convert_to_tensor(weights)

outputArray_tf = tf.matmul(inputArray_tf,weights_tf)

nnWeights_tf = tf.convert_to_tensor(nnWeights)
deltaweights_tf = tf.convert_to_tensor(deltaweights)
deltainput_tf = tf.convert_to_tensor(deltainput)
deltaoutput_tf = tf.convert_to_tensor(deltaoutput)

historyTrainTF=[]
i = 1
while i <= epochs:
    #with tf.Session() as session:  
    with tf.compat.v1.Session() as session:
        with tf.GradientTape() as tape:
            #Forward pass:
            tape.watch(nnWeights_tf)            
            nnOutput_tf = tf.matmul(inputArray_tf,nnWeights_tf)
            #print(nnOutput)
            #print(nnOutput_tf.eval())           
            lossTF = tf.reduce_mean(0.5*tf.square(nnOutput_tf - outputArray_tf))           
            historyTrainTF.append(lossTF.eval())
            #print(lossTrain)
            #print(lossTF.eval())
            tape.watch(lossTF)
            #Print Loss every 50 epochs: 
            if(i%10==0):
                print("Epoch: " + str(i) + " Loss (TF): " + "{0:.3f}".format(lossTF.eval()))

            #Backpropagate           
            deltaweights_tf = tape.gradient(lossTF,nnWeights_tf)

            #Apply optimizer
            #print(deltaweights)
            #print(deltaweights_tf.eval())
            nnWeights_tf -= deltaweights_tf*learningRate

            #Start new epoch
            i = i+1

plt.plot(historyTrainTF)
plt.title('RMSE loss')
plt.ylabel('Loss')
plt.xlabel('Epoch')
plt.legend(['train'], loc='upper right')
plt.show()

