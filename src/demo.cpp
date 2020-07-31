#include "cvplot.h"
#include <direct.h>

void create_directory(const std::string dir);

void build_figure(const std::string folder, const std::string alias);

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

	auto s2 = cvplot::Series("series-2", cvplot::chart::Line)
		.SetRenderColor(cvplot::color::DarkOrange)
		.AddValues({ 2,32,5,9,7,12,10,1,13,5,19,11,25,2,27,3,32,10 });

	const int N = 10000;
	for (int i = 0; i < N; ++i)
	{
		auto x = 20.0 * (N / 2 - i + 1) / N;
		auto y = x * x;
		s2.AddValues({ x,y });
	}

	auto s3 = cvplot::Series("series-3", cvplot::chart::Scatter, cvplot::marker::Diamond)
		.SetRenderColor(cvplot::color::Red)
		.AddValues({ 12,2,5,25,-7,12,15,1 })
		.AddValues({ 6,2 });

	auto s4 = cvplot::Series("series-4", cvplot::chart::Elevation);
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			s4.AddValues({ (double)i,(double)j,(double)(100 - (i + 1) * j * j) });
		}
	}

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
		.SetTextColor(cvplot::color::Blue)
		.EnableGrid(true)
		.SetGridColor(cvplot::color::LightGray)
		.AddSeries(s2)
		.AddSeries(s3);

	auto v3 = cvplot::View()
		.SetTitle("v-1-3")
		.SetXLabel("test-3x")
		.SetYLabel("test-3y")
		.SetSize({ 800, 600 })
		.SetTextColor(cvplot::color::OrangeRed)
		.AddSeries(s4);

	auto f = cvplot::Figure()
		.SetLayout(1, 3)
		.SetSize({ 1800,600 })
		.SetView(v1, 1, 1)
		.SetView(v2, 1, 2)
		.SetView(v3, 1, 3);

	//f.Show();
	//f.Save("figure.png");
	create_directory("dump\\");
	f.Dump("dump\\", "fig1");
	
	build_figure("dump\\", "fig1");
}


void create_directory(const std::string dir)
{
	if (dir.empty())
	{
		return;
	}

	auto pos = dir.find_first_of(':');
	if (pos == dir.npos)
	{
		pos = 0;
	}
	else
	{
		pos += 1;
	}

	pos = dir.find('\\', pos + 1);
	if (pos == dir.npos)
	{
		pos = dir.find('/', pos + 1);
	}

	if (pos == dir.npos)
	{
		_mkdir(dir.c_str());
		return;
	}

	do
	{
		auto s = dir.substr(0, pos);
		_mkdir(s.c_str());
		pos = dir.find('\\', pos + 1);
		if (pos == dir.npos)
		{
			pos = dir.find('/', pos + 1);
		}
	} while (pos != dir.npos);
}

void build_figure(const std::string folder, const std::string alias)
{
	cvplot::Figure fig;
	fig.Load(folder, alias);
	fig.Show();
	fig.Save(folder + alias + ".png");
}