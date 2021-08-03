data(rectangle(one,4,5)).
data(rectangle(two,6,8)).
compute(A,Sq):-data(rectangle(A,B,C)),
    Sq is (B*C).
compute(A,B,Sq):- Sq is (A*B).