:- dynamic cash/3.
:- dynamic s/2.
%инициализация суммы для конкретного лица X
init(X):-s(X,Y),retract(s(X,Y)), fail.    % удаляем старое значение
init(X):-assert(s(X,0)).           % добавляем сумму = 0

%добавление или вычет из общей суммы в зависимости от типа операции
sum(X,'in',C):- s(X,A), B is A + C, retract(s(X,A)), assert(s(X,B)),fail.
sum(X,'out',C):- s(X,A), B is A - C, retract(s(X,A)), assert(s(X,B)),fail.

% сама процедура, которая осуществляет подсчёт остатка на счету клиента
oper(X):-init(X), cash(X,Y,C), sum(X,Y,C).
oper(X):-s(X,S), write(S).

 