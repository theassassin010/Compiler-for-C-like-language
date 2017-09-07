// Boolean condition cannot be an arithmetic expression.
void main();

main()
{
	int a, b, c;
	int c2,d;
	float x,y,z;
	a = 3; b = 5; c = 10;
	if (a + b < c)
		a = a <= 6 ? a+1 : -3;
	if (a==3)
		a=3;
	else
		b=2;
	if (a==3)
		a=3;
	else
		{
			b=2;
		}
	if (a==3)
	{
		a=3;
	}
	else
	{
		b=2;
	}
	if (a==3)
	{
		a=3;
	}
	else
		b=2;
	if (a==3)
	{
		a=3;
	}
	while(a<2)
		a=2;
	while(a<2)
	{
		a=2;
	}
	do
		a=3;
		while(a>2);
	do
		{
			a=3;
		}
		while(a>2);


	a = (3 < b && 3==b && 3 != c || 4>=c || !(b <= c)) ? ( a+b )*(a/b) : (a-b)*(a*c);

	//I am here

	if (a==c)
		while (b==c2)
			do
				c=4;
				while(c != 4);

}

























