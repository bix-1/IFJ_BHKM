package main

func foo (s float64) (float64) {

return s
}

func main () { 

   
        a:= 5.5
        b:= (a*2.0) - ((foo(2.0) + foo(2.5)) / foo(0.5))

}
