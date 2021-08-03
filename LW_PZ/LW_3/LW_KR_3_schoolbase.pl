:- dynamic школьный_товарищ/1.
cl :- школьный_товарищ(X), retract(школьный_товарищ(X)), fail.
in :- repeat, write("Введите имя"), nl, read(X), assert(школьный_товарищ(X)),
    X = конец, !, retract(школьный_товарищ(X)), out.
out :- школьный_товарищ(Y), write(Y), nl, fail.