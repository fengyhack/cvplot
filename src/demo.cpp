#include "cvplot.h"

int main()
{
	auto s1 = cvplot::Series("series-1", cvplot::chart::Bar)
		.SetRenderColor(cvplot::color::SkyBlue);

	auto s11 = cvplot::Series("series-1-1", cvplot::chart::Bar)
		.SetRenderColor(cvplot::color::OrangeRed);

	auto s2 = cvplot::Series("series-2", cvplot::chart::Line, cvplot::marker::Circle)
		.SetRenderColor(cvplot::color::DarkOrange);

	auto s3 = cvplot::Series("series-3", cvplot::chart::Scatter, cvplot::marker::Diamond)
		.SetRenderColor(cvplot::color::Red);

	auto s4 = cvplot::Series("series-4", cvplot::chart::Elevation);

	s1.AddValue(1);
	s1.AddValues({ 3,2,5,1,8,9 });
	s11.AddValues({ 7,2,0,3,6,11,1 });
	s2.AddValues({ 2,32,5,9,7,12,10,1,13,5,19,11,25,2,27,3,32,10 });
	s3.AddValues({ 2,2,5,9,7,12,10,1 });
	s3.AddValues({ 6,2 });
	s4.AddValues({ 1, 5,25,2,3,58,3,4,180,5,2,80,3,8,100,8,1,200 });

	auto v1 = cvplot::View()
		.SetTitle("v-1-1")
		.SetSize({ 800, 800 })
		.SetXLabel("test-1x")
		.SetYLabel("test-1y")
		.SetTextColor(cvplot::color::DarkOrange);

	auto v2 = cvplot::View()
		.SetTitle("v-1-2")
		.SetSize({ 800, 800 })
		.SetXLabel("test-2x")
		.SetYLabel("test-2y")
		.EnableGrid(true)
		.SetGridColor(cvplot::color::LightGray);

	auto v3 = cvplot::View()
		.SetTitle("v-1-3")
		.SetSize({ 800, 800 })
		.SetXLabel("test-3x")
		.SetYLabel("test-3y");

	v1.AddSeries(s1);
	v1.AddSeries(s11);
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
	f.Save("figure.png", true);
}
