package main 

func foo (s int) (int) {

if s <= 3 {
s = s + 1
return foo(s) * 2
} else {
return 1
}

}
func main () { 


        a:= 0
        a = foo(1)
       print(a) // 8


}
