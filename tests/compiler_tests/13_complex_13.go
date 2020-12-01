package main


func m (n int) (int) {
inc:= n + 1 
if n >= 5 {  
return 1	//third return 1 
} else { 
a:= 0
a = m(inc) 
return n * a //second return 4 * 1, first return 4 * 3
}

}


func main() {
        a:= 0
        a = m(3)
        print(a) // 12
}
