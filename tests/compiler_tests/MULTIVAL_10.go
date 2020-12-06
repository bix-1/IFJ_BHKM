package main

func foo (s string) (n string, x string, p string) {
n = s + "n"
x = s + "x"
return
}

func main () {
        n, x:= foo("")
}

