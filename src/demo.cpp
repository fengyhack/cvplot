#include "cvplot.h"

int main()
{
    auto s1 = cvplot::Series("bar", cvplot::chart::Bar);
	auto s2 = cvplot::Series("line", cvplot::chart::Line, cvplot::marker::Circle);
	auto s3 = cvplot::Series("scatter", cvplot::chart::Scatter, cvplot::marker::Cross);
	auto s4 = cvplot::Series("elevation", cvplot::chart::Elevation);

	s1.AddValue(1);
	s1.AddValues({ 3,2,5,1,8,6 });
	s2.AddValues({ 2,32,5,9,7,12,10,1,13,5,19,11,25,2,27,3,32,10 });
	s3.AddValues({ 2,2,5,9,7,12,10,1 });
	s3.AddValues({ 6,2 });
	s4.AddValues({ 1, 5,25,2,3,58,3,4,18,5,2,80,3,8,100,8,1,20 });

	auto v1 = cvplot::View().SetTitle("1-1").SetSize({ 800, 800 });
	auto v2 = cvplot::View().SetTitle("1-2").SetSize({ 800, 800 });
	auto v3 = cvplot::View().SetTitle("1-3").SetSize({ 800, 800 });

	v1.AddSeries(s1);
	v2.AddSeries(s2);
	v2.AddSeries(s3);
	v3.AddSeries(s4);
	
	cvplot::Figure f;
	f.SetLayout(1, 3);
	f.SetSize({ 1800,600 });
	
	f.SetView(v1, 1, 1);
	f.SetView(v2, 1, 2);
	f.SetView(v3, 1, 3);

	f.Show();
	f.Save("figure.png");
}
