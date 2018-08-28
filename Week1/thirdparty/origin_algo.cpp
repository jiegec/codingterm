 /**************************************************************************
***
*** Copyright (c) 2018 Regents of the Tsinghua University,
***               Hailong Yao and Weiqing Ji
***
***  Contact author(s): jwq18@mails.tinghua.edu.cn, hailongyao@mail.tsinghua.edu.cn
***  Original Affiliation:   EDA Lab., Department of Computer Science and Technology, Tsinghua University.
***
***  Permission is hereby granted, free of charge, to any person obtaining
***  a copy of this software and associated documentation files (the
***  "Software"), to deal in the Software without restriction, including
***  without limitation
***  the rights to use, copy, modify, merge, publish, distribute, sublicense,
***  and/or sell copies of the Software, and to permit persons to whom the
***  Software is furnished to do so, subject to the following conditions:
***
***  The above copyright notice and this permission notice shall be included
***  in all copies or substantial portions of the Software.
***
*** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
*** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
*** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
*** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***
***
***************************************************************************/


 /*
 *Created on 2018-8-27  
 *Author:Weiqing_Ji
 *Version 1.0 
 *Title: 流速计算程序
 */
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>  
#include <math.h>
using namespace std;
#define NAX 0.000000001

struct edge
{
	double v; //流速
	double leng; //管道长度
	int n1,n2;
}edges[200];
struct node
{          
	vector<int> elist;  //点所连接的边，按照顺时针的顺序存储下来

}nodes[200];

vector< vector<double> >rect(200);//存储行列式

bool *fr; //进行遍历

int n,EDGESUM,NODESUM;  //记录网格边长


bool setedgelength(int x, double leng)
{
	edges[x].leng = leng;
}

void addrect(vector<double> &tmp){
	rect.push_back(tmp);
}

int getdirline(int x, int y, int dir){
	int e;
	int sum = x*8+y;
	if (dir == 0){
		if (y==0){
			if (edges[EDGESUM-5].n2 == sum)
				e = EDGESUM-5;
			else if (edges[EDGESUM-4].n2 == sum)
				e = EDGESUM-4;
			else 
				e = EDGESUM;
		}
		else 
			e=(x*(n-1)+y-1);
	}else if (dir == 1){
		if (x==n-1)
			e=EDGESUM;
		else 
			e=n*n-n+x*n+y;
	}else if (dir == 2){
		if (y==n-1){
			if (edges[EDGESUM-3].n1 == sum)
				e = EDGESUM-3;
			else if (edges[EDGESUM-2].n1 == sum)
				e = EDGESUM-2;
			else if (edges[EDGESUM-1].n1 == sum)
				e = EDGESUM-1;
			else 
				e = EDGESUM;
		}
		else
			e=(x*(n-1)+y);
	}else if (dir == 3){
		if (x==0)
			e=EDGESUM;
		else
			e=n*n-n+(x-1)*n+y;
	}
	return e;
}

bool existdir(int x, int y, int dir)
{
	int e = getdirline(x,y,dir);
	if (e>=EDGESUM)
		return false;
	return (edges[e].leng!=0);
}

void recursionline(int x, int y, int dir, vector<double> &tmp,int end){
	int e = getdirline(x,y,dir);
	// cout<<"recursionline  "<<x<<" "<<y<<" "<<e<<"  "<<edges[e].n1<<" "<<edges[e].n2<<endl;
	if (e>EDGESUM-6)
		return;
	int nodeno = x*n+y;
	int x1;
	if (edges[e].n1 == nodeno){
		tmp[e] = edges[e].leng;
		x1 = edges[e].n2;
	}
	else{
		tmp[e] = -edges[e].leng;
		x1 = edges[e].n1;
	}

	if (e == end){
		addrect(tmp);
		return;
	}
	if (dir == 0){
		y--;
	}else if (dir == 1){
		x++;
	}else if (dir == 2){
		y++;
	}else if (dir == 3){
		x--;
	}
	for(int i=0; i<3; i++)
	{
		int newdir = dir+1-i;
		newdir = (newdir+4)%4;
		if (existdir(x,y,newdir))
		{
			recursionline(x,y,newdir,tmp,end);
			break;
		}
	}
}

bool recursionrect(int x, int y, vector<double> &tmp,int end){
	int xx,yy;
	for(int i=1; i<5; i++)
	{
		int e =  getdirline(x,y,i%4);
		int dir = i%4;
		// cout<<"recursionrect  "<<x<<" "<<y<<" "<<e<<"  "<<i%4<<endl;
		if (fr[e])
			continue;
		if (edges[e].leng==0)
			continue;
		fr[e] = true;
		int nodeno = x*n+y;
		int x1;
		if (edges[e].n1 == nodeno){
			tmp[e] = edges[e].leng;
			x1 = edges[e].n2;
		}
		else{
			tmp[e] = -edges[e].leng;
			x1 = edges[e].n1;
		}
		if (e == end){
			// cout<<"alsdhjkagjkdhasdhasjkld"<<endl;
			return true;
		}
		if (e>EDGESUM-5){
			tmp[e] = 0;
			continue;
		}

		if (dir == 0){
			xx=x;yy=y-1;
		}else if (dir == 1){
			xx=x+1;yy=y;
		}else if (dir == 2){
			xx=x;yy=y+1;
		}else if (dir == 3){
			xx=x-1;yy=y;
		}
		if (recursionrect(xx,yy,tmp,end))
			return true;
		tmp[e] = 0;
	}
	return false;
}
void findline(int x, int y,int t){
	// cout<<"findline  "<<x<<" "<<y<<" "<<t<<endl;
	vector<double> tmp(EDGESUM+1,0);
	recursionline(x,y,1,tmp,t);
}

void findrect(int x1){
	vector<double> tmp(EDGESUM+1,0);
	for (int i=0; i<EDGESUM; i++)
		fr[i] = false;
	recursionrect(edges[x1].n2/n,0,tmp,EDGESUM-1);
	// for (int i=0; i<EDGESUM; i++)
	// 	cout<<tmp[i]<<" ";
	// cout<<endl;
	vector<double> temp(EDGESUM+1,0);
	for (int i=0; i<EDGESUM; i++)
		fr[i] = false;
	recursionrect(edges[x1].n2/n,0,temp,EDGESUM-2);
	// for (int i=0; i<EDGESUM; i++)
	// 	cout<<tmp[i]<<" ";
	// cout<<endl;
	vector<double> tep(EDGESUM+1,0);
	for (int i=0; i<EDGESUM; i++)
		fr[i] = false;
	recursionrect(edges[x1].n2/n,0,tep,EDGESUM-3);
	// for (int i=0; i<EDGESUM; i++)
	// 	cout<<tmp[i]<<" ";
	// cout<<endl;
	vector<double> emp(EDGESUM+1,0);
	for (int i=0; i<EDGESUM+1; i++)
		emp[i] = tmp[i]-temp[i];
	rect.push_back(emp);
	vector<double> tem(EDGESUM+1,0);
	for (int i=0; i<EDGESUM+1; i++)
		tem[i] = tep[i]-temp[i];
	rect.push_back(tem);
}

void initrect(){    //统计行列式的值
	for (int i=0;i<EDGESUM-5; i++)	//不存在的管道液体流速为0
		if (edges[i].leng == 0)
		{
			// cout<<"exist  "<<i<<endl;
			vector<double> tmp(EDGESUM+1,0);
			tmp[i]=1;
			addrect(tmp);
		}
	for (int i=0; i<NODESUM-2; i++)  //首先根据基尔霍夫定律，统计所有的电流的相等关系
	{
		int number = 0;
		vector<double> tmp(EDGESUM+1,0);
		for (int j=0; j<nodes[i].elist.size(); j++)
		if (edges[nodes[i].elist[j]].leng!=0)
		{
			number++;
			if (edges[nodes[i].elist[j]].n1 == i)
				tmp[nodes[i].elist[j]] = 1;
			else
				tmp[nodes[i].elist[j]] = -1;
		}
		tmp[EDGESUM] = 0;
		if (number>0)
			addrect(tmp);
	}

	// cout<<11123131<<" "<<rect.size()	<<endl;
	for (int i=0; i<n-1; i++)
		for (int j=0; j<n-1; j++)
		{
			int t = i*(n-1)+j; 
			int m = n*n-n+i*n+j; 
			if (edges[t].leng !=0 && edges[m].leng !=0)
					findline(i,j,t);
		}

	// cout<<11123131<<" "<<rect.size()	<<endl;
	findrect(EDGESUM-4);

	vector<double> tmp(EDGESUM+1,0);
	tmp[EDGESUM-5] = 1;
	tmp[EDGESUM] = 200;
	addrect(tmp);
	vector<double> temp(EDGESUM+1,0);
	temp[EDGESUM-4] = 1;
	temp[EDGESUM] = 200;
	addrect(temp);

}










int min (int a, int b)  //求最小值
{
	if (a>b)
		return b;
	else
		return a;
}

double GreatestCommonDivisor(double a, double b)
{
	// cout<<"GreatestCommonDivisor "<<a<<" "<<b<<endl;
    double t;
    if (a < b)
    {
        t = a;
        a = b;
        b = t;
    }    
    if (b==1)
    	return 1;
    while (abs(b) >= NAX)
    {
        t = fmod(a , b);
        a = b;
        b = t;
    }
    // cout<<"a  "<<a<<endl;
    return a;
}

double LeastCommonMultiple (double a, double b)

{
	double c = GreatestCommonDivisor(a,b);
    double t = a  /c * b;
    return t;

}

void getans()    //计算行列式的值
{
	int n = rect.size();

	for (int i=0; i<n; i++)
		for (int j=0; j<EDGESUM+1; j++)
			if (abs(rect[i][j])<NAX)
				rect[i][j] = 0;

	int num = 0;
	for (int i=0; i<n; i++){

			// string s = std::to_string(num);

			// string name = "test\\test"+s+".txt";
			// ofstream fmt(name);
			// for (int gg=0; gg<n; gg++){
			// 	for (int j=0; j<EDGESUM+1; j++)
			// 		fmt<<rect[gg][j]<<" ";
			// 	fmt<<endl;
			// }
			// fmt.close();

		if (abs(rect[i][num]) <NAX){
			int mjj = 0;
			for (int j=i+1; j<n; j++)
				if ( !(abs(rect[j][num])<NAX))
				{
					mjj++;
					// cout<<"num "<<num<<" "<<j<<" "<<rect[j][num]<<endl;
					for (int k=0; k<EDGESUM+1; k++)
					{
						double t = rect[j][k];
						rect[j][k] = rect[i][k];
						rect[i][k] = t;
					}
					break;
				}
			// if (mjj == 0)
			// 	cout<<"454354354354354"<<endl;
		}
		for (int j=0; j<n; j++)
			if (i!=j && (abs(rect[j][num])>NAX))
				{
					double ml = LeastCommonMultiple(abs(rect[j][num]),abs(rect[i][num]));
					double t = ml/rect[j][num];
					double kt = ml/rect[i][num];
					for (int k=EDGESUM; k>=0; k--){
						rect[j][k] = rect[j][k]*t;
						rect[j][k] = (abs(rect[j][k])<NAX)?0:rect[j][k];
					}
					for (int k=EDGESUM; k>=num; k--)
					{
						rect[j][k] -= kt*rect[i][k];
						rect[j][k] = (abs(rect[j][k])<NAX)?0:rect[j][k];
					}


				}

			
			num ++;
			for (int j=0; j<n; j++){
				double common = 0;
				for (int k=0; k<EDGESUM+1; k++)
				if ((abs(rect[j][k]) > NAX)){
					if (abs(common) < NAX)
						common = abs(rect[j][k]);
					else
						common = GreatestCommonDivisor(common,abs(rect[j][k]));
				}
				if (common!=0)
					for (int k=0; k<EDGESUM+1; k++){
						rect[j][k] = rect[j][k]/common;
						rect[j][k] = (abs(rect[j][k])<NAX)?0:rect[j][k];
					}
			}
		if (num==EDGESUM)
			break;
	}
	
	num = 0;
	for (int i=0; i<EDGESUM; i++)
	{
		edges[num].v = double(rect[i][EDGESUM])/double(rect[i][num]);
		if (edges[num].v<0)
		{
			edges[num].v = -edges[num].v;
			int tm = edges[num].n1;
			edges[num].n1 = edges[num].n2;
			edges[num].n2 = tm;
		}
		num++;
		if (num==EDGESUM)
			break;
	}

	// for (int i=0; i<EDGESUM; i++){
	// 	// cout<<i<<":   "<<edges[i].n1<<"  "<<edges[i].n2<<endl;
	// 	cout<<i<<":   "<<edges[i].v<<endl;
	// }

}


vector<double> caluconspeed(int num, vector<double>&length, int i1, int i2, int o1, int o2, int o3)
{
	rect.clear();
	n = num;
	EDGESUM = 2*n*n-2*n+5;
	NODESUM = n*n+2;
	int n1 = 0;
	int n2 = 1;
	fr = new bool[EDGESUM];
	for (int i=0; i<n*n-n; i++)
	{
		edges[i].n1 = n1;
		edges[i].n2 = n2;
		nodes[n1].elist.push_back(i);
		nodes[n2].elist.push_back(i);
		n1++;n2++;
		if (n2%n==0)
		{
			n1++;n2++;
		}
	}
	n1 = 0;
	n2 = n1+n;
	for (int i=n*n-n; i<2*n*n-2*n; i++)
	{
		edges[i].n1 = n1;
		edges[i].n2 = n2;
		nodes[n1].elist.push_back(i);
		nodes[n2].elist.push_back(i);
		n1++;n2++;
	}

	edges[2*n*n-2*n+0].n1 = n*n;
	edges[2*n*n-2*n+0].n2 = (i1)*n;
	nodes[n*n].elist.push_back(2*n*n-2*n+0);
	nodes[(i1)*n].elist.push_back(2*n*n-2*n+0);

	edges[2*n*n-2*n+1].n1 = n*n;
	edges[2*n*n-2*n+1].n2 = (i2)*n;
	nodes[n*n].elist.push_back(2*n*n-2*n+1);
	nodes[(i2)*n].elist.push_back(2*n*n-2*n+1);

	edges[2*n*n-2*n+2].n1 = (o1+1)*n-1;
	edges[2*n*n-2*n+2].n2 = n*n+1;
	nodes[(o1+1)*n-1].elist.push_back(2*n*n-2*n+2);
	nodes[n*n+1].elist.push_back(2*n*n-2*n+2);

	edges[2*n*n-2*n+3].n1 = (o2+1)*n-1;
	edges[2*n*n-2*n+3].n2 = n*n+1;
	nodes[(o2+1)*n-1].elist.push_back(2*n*n-2*n+3);
	nodes[n*n+1].elist.push_back(2*n*n-2*n+3);

	edges[2*n*n-2*n+4].n1 = (o3+1)*n-1;
	edges[2*n*n-2*n+4].n2 = n*n+1;
	nodes[(o3+1)*n-1].elist.push_back(2*n*n-2*n+4);
	nodes[n*n+1].elist.push_back(2*n*n-2*n+4);

	// setedgelength(2*n*n-2*n,1);
	// setedgelength(2*n*n-2*n+1,1);
	// setedgelength(2*n*n-2*n+2,1);
	// setedgelength(2*n*n-2*n+3,1);
	// setedgelength(2*n*n-2*n+4,1);	
	
	for (int i=0; i<2*n*n-2*n+5; i++)
	{
		setedgelength(i,length[i]);
	}
	edges[EDGESUM-4].v = 200;
	edges[EDGESUM-5].v = 200;

	initrect();
	// cout<<11123131<<" "<<rect.size()	<<endl;
	getans();
	vector<double> v(3,0);
	v[0] = edges[EDGESUM-3].v;
	v[1] = edges[EDGESUM-2].v;
	v[2] = edges[EDGESUM-1].v;
	return v;
}

int main(int argc, char ** argv)
{
	int n;
	cin>>n;
	vector<double> leng(2*n*n-2*n,0);
	for (int i=0; i<2*n*n-2*n; i++)
	{
		cin>>leng[i];
	}
	vector<double> ans = caluconspeed(n,leng,2,5,0,4,7);
	for (int i=0; i<3;i++)
		cout<<ans[i]<<endl;
	return 0;
}
