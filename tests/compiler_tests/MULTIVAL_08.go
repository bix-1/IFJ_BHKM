package main

func foo (s string) (n string, x string) {
n = s + "n"
x = s + "x"
return
}

func main () {
        n, _:= foo("")
}

