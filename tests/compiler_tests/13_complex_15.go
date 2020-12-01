package main
func m (n int) (int) {
inc:= n + 1 
if n >= 5 {  
return 1 //second call is 1
} else { 
a:= 0
a = m(inc) // a = 1
return n + a // first call is 4 + 1
}

}


func main() {
        a:= 0
        a = m(4)
        print(a) // 5
}
