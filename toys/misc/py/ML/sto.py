from stochastic.processes.discrete import BernoulliProcess

bp = BernoulliProcess(p=0.6)
s = bp.sample(16)
success_probability = bp.p

print(success_probability)

