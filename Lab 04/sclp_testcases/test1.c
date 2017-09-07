// Empty statement in if statement is allowed.
void main();

main()
{
	// float f = -5.5, g = 6.6;
	float f;
	float g;
	int a,b,c,d;
	if (f<g)
	{
		f=3.4;
	}
	else if(f<g)
	{

	}
	else
	{
		
	}
	// // if(f<g); // Shouldn't accept
	if((a+b)==(b+c))
	{
		c = 5;
		if(c<4) // No need of bracket for if
			c = 3;
	}
	d = a<4?b:c;
	if(c<=4)
		if(c>4)
		{

		}
	if(c>=6)
	{

	}
	else
		c=4;
	if(c==3)
		c=2;
	else
		d=3;
	// // while(c!=3)
	// // {

	// // }
	// // while(c<3)
	// // 	c=4;
	// // if(a+b) // Can't use this
	// // {

	// // }
	if((a>2)&&(b==2))
	{

	}
	if(a>2)
	{

	}
	if((a==2)||(b==2))
	{

	}
	if(!(a==2))
	{

	}
	// a = (b>2)?f:g; // Error
	f = ((b>2))?(f):(g);
	a = -b;
}
