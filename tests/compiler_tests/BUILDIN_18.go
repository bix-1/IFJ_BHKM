package main

func main () {
        a:="1234\n\x44"
	b:=0
	c:=0
	d:=0
	e:=0
	f:=0
	g:=0
	i:=0
	k:=0

	b,g = ord(a, 0)
	if g == 0{
	print("OK", b)
	} else {
	print("fail")
	}

	c,g = ord(a, 1)
        if g == 0{
        print("OK", c)
        } else {
        print("fail")
        }

	d,g = ord(a, 2)
        if g == 0{
        print("OK", d)
        } else {
        print("fail")
        }

	e,g = ord(a, 3)
        if g == 0{
        print("OK", e)
        } else {
        print("fail")
        }

	i,g = ord(a, 4)
        if g == 0{
        print("OK", i)
        } else {
        print("fail")
        }

	k,g = ord(a, 5)
        if g == 0{
        print("OK", k)
        } else {
        print("fail")
        }

}

