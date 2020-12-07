package main

func main () {

a:=5
b:=0
c:=3.5
d:=1.0e-1
e:= 5.99
//ok
if (float2int(e) == 5) {
	a := 2.2
	//ok
	if float2int(a) >= 3 {
	print("nenastanem")
	} else {
	print("nastanem\t\t\\\n")
	}

	c = 10.2 // c = 10.2
}
/*            ok         */
if (c >= int2float(a)) {
print("Stane")
} else {
e = int2float(10)	// e = 10.0
}

/* O.K */
if int2float(a) == e {

b = float2int(d) // b = 0
}

a = a * 4 // ok
e = lls(3)  - 2.0 // c = 0

if c == 0.0 {
print("hotovo")
}

}

func lls (n int) (float64) {
n = n + 1
s:= int2float(n) - 2.0
return s
}
