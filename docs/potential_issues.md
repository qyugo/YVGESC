**POTENTIAL ISSUES**

Power Inductor 

A smaller 47uH power inductor is used, which may cause some amount of unwanted input fluctuation compared to a calculated 51.4uH. 

The Wurth Elektronik 7447779147 model was selected for its sourceability and smaller size (7.3 x 7.3mm) compared to many 12x12 counterparts to fit on a smaller final board. It is worth considering that a 47μH inductor results in a 43.8% ripple current, Generally, a 20-40% allowance is advised as a design heuristic for minimizing losses due to AC ripple, a slight overstep would transfer in the form of heat to the output capacitor COUT, and perhaps a transition from continuous conduction (CCM) to discontinuous conduction mode (DCM).
 
However, the minimum ripple generation (RA, CA, CB) is arranged for the LM5164 buck regulator.

<img width="941" height="473" alt="Screenshot 2026-09-05 at 2 44 44 PM" src="https://github.com/user-attachments/assets/38e049a9-4435-4c90-9dad-8096c07491b8" />
