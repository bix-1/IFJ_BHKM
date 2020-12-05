package main

func main () {

a:=65
b:=66
c:=255
d:=0
e:=10
f:="x"
g:=0

f, g = chr(a)
if g == 0 {
print("OK")
} else {
print("FAIL")
}

f, g = chr(b)
if g == 0 {
print("OK")
} else {
print("FAIL")
}

f, g = chr(c)
if g == 0 {
print("OK")
} else {
print("FAIL")
}

f, g = chr(d)
if g == 0 {
print("OK")
} else {
print("FAIL")
}

f, g = chr(e)
if g == 0 {
print("OK")
} else {
print("FAIL")
}

}

