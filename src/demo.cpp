#include "cvplot.h"

int main()
{
	auto s1 = cvplot::Series("series-1", cvplot::chart::Bar)
		.SetRenderColor(cvplot::color::SkyBlue)
		.AddValue(1)
		.AddValues({ 3,2,5,1,8,9 });

	auto s11 = cvplot::Series("series-1-1", cvplot::chart::Bar)
		.SetRenderColor(cvplot::color::OrangeRed)
		.AddValues({ 7,2,0,3,6,11,1 });

	auto s12 = cvplot::Series("series-1-2", cvplot::chart::Trends)
		.SetMarkerType(cvplot::marker::Square)
		.SetRenderColor(cvplot::color::Blue)
		.AddValues({ 3,0,5,8,2,3,6 });

	auto s2 = cvplot::Series("series-2", cvplot::chart::Line, cvplot::marker::Circle)
		.SetRenderColor(cvplot::color::DarkOrange)
		.AddValues({ 2,32,5,9,7,12,10,1,13,5,19,11,25,2,27,3,32,10 });

	auto s3 = cvplot::Series("series-3", cvplot::chart::Scatter, cvplot::marker::Diamond)
		.SetRenderColor(cvplot::color::Red)
		.AddValues({ 2,2,5,9,7,12,10,1 })
		.AddValues({ 6,2 });

	auto s4 = cvplot::Series("series-4", cvplot::chart::Elevation)
		.AddValues({ 1,5,25,2,3,58,3,4,180,5,2,80,3,8,100,8,1,200 });

	auto v1 = cvplot::View()
		.SetTitle("v-1-1")
		.SetSize({ 600, 600 })
		.SetXLabel("test-1x")
		.SetYLabel("test-1y")
		.SetTextColor(cvplot::color::DarkOrange)
		.AddSeries(s1)
		.AddSeries(s11)
		.AddSeries(s12);

	auto v2 = cvplot::View()
		.SetTitle("v-1-2")
		.SetSize({ 600, 600 })
		.SetXLabel("test-2x")
		.SetYLabel("test-2y")
		.EnableGrid(true)
		.SetGridColor(cvplot::color::LightGray)
		.AddSeries(s2)
		.AddSeries(s3);

	auto v3 = cvplot::View()
		.SetTitle("v-1-3")
		.SetSize({ 600, 600 })
		.SetXLabel("test-3x")
		.SetYLabel("test-3y")
		.AddSeries(s4);

	auto f = cvplot::Figure()
		.SetLayout(1, 3)
		.SetSize({ 1800,600 })
		.SetView(v1, 1, 1)
		.SetView(v2, 1, 2)
		.SetView(v3, 1, 3);

	f.Show();
	f.Save("figure.png");
}
