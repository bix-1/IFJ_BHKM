package main
func main () {
s:= 0
s = foo(s)
if s == 20 {
	print("succ")
}
print(s) // 20

for ;s >= 10; s = s - 1 {
print(s) // 20
}
}

func foo (s int) (int) {

for a:=0.5 ; a <= 10.0 ; a = a + 0.5{
s = s + 1
}
return s
}
