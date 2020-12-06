package main

func main () {
        a:= false
        b:= 4.0e-1
        a = foo(false)
        if b <= 100.2 || a {
        print("OK", b, a)
        }

}

func foo (s bool) (bool) {
k:= false
if (s == false) {
k = true
}

return k || true
}
