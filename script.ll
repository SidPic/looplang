;var global

func sqr x
    return x*x
    say "AAAAAAAAAAA!!"
end

func dif a b
    return a - b
end


func hello
    var x "Hello world!"
    say x " sqr of 1, 2, 3: " sqr 1 " " sqr 2 " " sqr dif (6-1) 2

    if 1 = 2
        say "IF 1 = 2"
    elif 1 = 3
        say "ELIF 1 = 3"
    elif 1 = 1
        say "ELIF 1 = 1"
        if 2 = 3
            say "IF 2 = 3"
        elif 2 = 2
            say "ELIF 2 = 2"
        endif
    else
        say "E_L_S_E"
    endif

    say "IF_ELIF_ELSE OK"

    if 1 = 5
        say "ELIF 1 = 5"
    else
        say "ELSE"
        if 1 = 1
            say "ELSE IF 1 = 1"
        endif
    endif

    say "IF_ELSE OK"

    var n 0
    loop n < 10 ;  n := 0..10 (возможный вариант, loop принимает bool, выражение под loop возвращает bool)
                ;  т.е. в данном случае '..' возвращает диапазон, ':=' обрабатывает его и возвращает bool
        say "# " n

        var j 0
        loop j < 2
            say j
            inc j
        endl

        inc n
    endl

    say "LOOP OK"
end

;корректно парсится
;var x, 10, object.func.a, asd, arr#(12 - 1), 11, 2 - (53 - (32 - 10 / o.a * (sum 1 2)) - 21) * 100
