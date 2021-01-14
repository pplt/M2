-- tests basic methods (e.g., arithmetic operations) for RRi implementation

FF := RRi_1000
assert(precision FF===1000)
I = interval(1,3)
assert sameinterval(I,interval(3,1))
assert (precision ring I === 53)
assert (precision interval(1,3_(RR_1000))==1000)
assert (precision interval(1000,1,3) == 1000)
assert (precision interval(1_(RR_1000),2_(RR_2000)) == 1000)
assert (precision (interval(100,1,3)+interval(200,2,4)) == 100)
assert (precision toRRi(300,2) == 300)
assert (precision toRRi(30) == 53)
assert (precision toRRi(10_(RR_100)) == 100)
assert (toRRi(4) == interval(4))
assert(toString FF === "RRi_1000")
assert(expression FF === new Subscript from {symbol RRi,1000})
assert(leftRR I === 1_(RR_53))
assert(rightRR I === 3_(RR_53))
assert(midpointRR I === 2_(RR_53))
J = toRRi 2
assert not(I<J or I>J or I==J or I>=J or I<=J)
assert contains(I,J)
assert (J <= J)
assert not(I <= I)
assert not (I==I)
assert (J==J)
assert (I===I)
assert not(I===interval(1,3))
assert (0<I)
assert (I<=interval(3,5))
assert (J<interval(3,5))
assert(leftRR sin J <= sin 2.0)
assert(rightRR sin J >= sin 2.0)
assert(leftRR sin J != rightRR sin J)
assert(sameinterval(I+J,J+I))
assert(sameinterval(I*J,J*I))
K = interval(-2,3)
assert(not sameinterval(K*K,K^2))
assert(widthRR sqrt I < 1)
assert(rightRR sqrt I > sqrt 3) -- this has to do with different rounding (mpfi vs mpfr)
assert(rightRR log exp toRRi 1 > 1)
assert(leftRR log exp toRRi 1 < 1)
assert sameinterval(abs(K),interval(0,3))
assert (midpointRR(I) == J)
assert (widthRR(I) == 2)
assert contains(numeric_100 piRRi,numeric_100 pi)
L = interval(-2/3,1.5_(RR_53))
M = interval(-4,2.7)
assert sameinterval(L+M,interval(-14/3,4.2))
assert contains(L-M,interval(-10.1/3,5.5))
assert contains(L*M,interval(-6,4.05))
assert contains(K*K,K^2)
assert not contains(K^2,K*K)
assert sameinterval(L/I,L)
assert sameinterval(L^2,interval(0,2.25))
assert contains(L^3,interval(-8/27,3.375))
assert contains(interval(-1,1),sin(L))
assert isFinite(I)
assert not isFinite(I/K)
assert isANumber(I)
assert isANumber(I/K)
assert not isInfinite(I)
assert isInfinite(I/K)
assert contains(acos cos(numeric_100 piRRi),numeric_100 pi)
assert (leftRR atan I == atan 1)
assert (rightRR atan I == atan 3)
assert (leftRR sinh I == sinh 1)
assert (acosh(I) >= 0)

--- TO FIX: ---------------------------------------------

-*
sqrt K -- produce a different error?
*-

-*
Ok to divide by 0, gives infinity.
Ok to take log of 0 (-infinity), just not negative values.
Can't take arc-trig of inappropriate intervals.
Some of these functions (like contains) should be isSubset
*-

-*
debug Core
errorDepth = 0 
FF[x]
-- start with... rawToRRi in d/interface.dd 
*-
