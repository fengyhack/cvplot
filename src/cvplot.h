#ifndef CVPLOT_H
#define CVPLOT_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <iomanip>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef byte
typedef unsigned char byte;
#endif

namespace cvplot
{
	class Color
	{
	public:
		Color(byte r, byte g, byte b, byte a = (byte)255)
		{
			r_ = r;
			g_ = g;
			b_ = b;
			a_ = a;
		}

		Color() : Color(0, 0, 0)
		{
			//
		}

		Color& Blend(byte a)
		{
			a_ = a;
			return *this;
		}

		Color& operator=(Color& rhs)
		{
			if (this != &rhs)
			{
				r_ = rhs.r_;
				g_ = rhs.g_;
				b_ = rhs.b_;
				a_ = rhs.a_;
			}
			return *this;
		}

		bool operator==(Color& rhs) const
		{
			return (r_ == rhs.r_ && g_ == rhs.g_ && b_ == rhs.b_ && a_ == rhs.a_);
		}

		bool operator!=(Color& rhs) const
		{
			return (r_ != rhs.r_ || g_ != rhs.g_ || b_ != rhs.b_ || a_ != rhs.a_);
		}

		Color Linear(double intensity)
		{
			if (intensity <= 0)
			{
				return Color(0, 0, 0, a_);
			}
			else if (intensity >= 255.0)
			{
				return Color(r_, g_, b_, a_);
			}

			int r = (int)(r_ * intensity + 0.5);
			int g = (int)(g_ * intensity + 0.5);
			int b = (int)(b_ * intensity + 0.5);
			r = r > 255 ? 255 : r;
			g = g > 255 ? 255 : g;
			b = b > 255 ? 255 : b;
			return Color(r, g, b, a_);
		}

		Color Lift(byte v)
		{
			int r = r_ > v ? r_ : v;
			int g = g_ > v ? g_ : v;
			int b = b_ > v ? b_ : v;
			return Color(r, g, b, a_);
		}

		Color Cut(byte v)
		{
			int r = r_ < v ? r_ : v;
			int g = g_ < v ? g_ : v;
			int b = b_ < v ? b_ : v;
			return Color(r, g, b, a_);
		}

		cv::Scalar ToScalar() const
		{
			return cv::Scalar(b_, g_, r_, a_);
		}

		cv::Vec4b ToVec4b() const
		{
			return cv::Vec4b(b_, g_, r_, a_);
		}

		cv::Vec3b ToVec3b() const
		{
			return cv::Vec3b(b_, g_, r_);
		}

	private:
		byte r_, g_, b_, a_;
	};

	namespace chart
	{
		typedef int Type;

		static const Type Bar = 1;
		static const Type Trends = 2;
		static const Type Line = 3;
		static const Type Scatter = 4;
		static const Type Elevation = 5;

		static int GetDimension(Type type)
		{
			int dim;

			switch (type)
			{
			case Bar:
			case Trends:
				dim = 1;
				break;
			case Line:
			case Scatter:
				dim = 2;
				break;
			case Elevation:
				dim = 3;
				break;
			default:
				dim = 1;
				break;
			}

			return dim;
		}
	}

	namespace marker
	{
		typedef int Type;

		static const Type None = 0;
		static const Type Cross = 1;
		static const Type Plus = 2;
		static const Type Star = 3;
		static const Type Circle = 4;
		static const Type Square = 5;
		static const Type Diamond = 6;
	}

	namespace color
	{
		static const byte GAMMA_LUT[] =
		{
			  0,  20,  28,  33,  38,  42,  46,  49,  52,  55,  58,  61,  63,  65,  68,  70,  72,  74,  76,  78,  80,  81,  83,  85,  87,  88,  90,  91,  93,  94,  96,  97,
			 99, 100, 102, 103, 104, 106, 107, 108, 109, 111, 112, 113, 114, 115, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 128, 129, 130, 131, 132, 133, 134, 135,
			136, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 147, 148, 149, 150, 151, 152, 153, 153, 154, 155, 156, 157, 158, 158, 159, 160, 161, 162, 162,
			163, 164, 165, 165, 166, 167, 168, 168, 169, 170, 171, 171, 172, 173, 174, 174, 175, 176, 176, 177, 178, 178, 179, 180, 181, 181, 182, 183, 183, 184, 185, 185,
			186, 187, 187, 188, 189, 189, 190, 190, 191, 192, 192, 193, 194, 194, 195, 196, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203, 203, 204, 205, 205,
			206, 206, 207, 208, 208, 209, 209, 210, 210, 211, 212, 212, 213, 213, 214, 214, 215, 216, 216, 217, 217, 218, 218, 219, 219, 220, 220, 221, 222, 222, 223, 223,
			224, 224, 225, 225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239,
			240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 254, 254, 255
		};

		static const Color Transparent = Color(255, 255, 255, 0);
		static const Color Red = Color(255, 0, 0);
		static const Color OrangeRed = Color(255, 69, 0);
		static const Color Orange = Color(255, 165, 0);
		static const Color DarkOrange = Color(255, 140, 0);
		static const Color Yellow = Color(255, 255, 0);
		static const Color LawnGreen = Color(124, 252, 0);
		static const Color Green = Color(0, 255, 0);
		static const Color Cyan = Color(0, 255, 255);
		static const Color SkyBlue = Color(135, 206, 235);
		static const Color Blue = Color(0, 0, 255);
		static const Color Black = Color(0, 0, 0);
		static const Color DarkGray = Color(32, 32, 32);
		static const Color Gray = Color(128, 128, 128);
		static const Color LightGray = Color(220, 220, 220);
		static const Color White = Color(255, 255, 255);

		typedef int Scheme;

		namespace scheme
		{
			static const Scheme Gray = 0;
			static const Scheme Red = 1;
			static const Scheme Green = 2;
			static const Scheme Blue = 3;
		}

		static Color Index(byte index, Scheme scheme)
		{
			byte r = 0;
			byte g = 0;
			byte b = 0;
			byte v = GAMMA_LUT[index];

			switch (scheme)
			{
			case scheme::Red:
				r = v;
				g = v > 0 ? (v >> 1) : 0;
				b = g;
				break;
			case scheme::Green:
				r = v > 0 ? (v >> 1) : 0;
				g = v;
				b = r;
				break;
			case scheme::Blue:
				r = v > 0 ? (v >> 1) : 0;
				g = r;
				b = v;
				break;
			default:
				r = v;
				g = v;
				b = v;
				break;
			}

			return Color(r, g, b);
		}

		static Color Gamma(cv::Vec4b color)
		{
			byte b = color[0];
			byte g = color[1];
			byte r = color[2];
			byte a = color[3];
			return Color(GAMMA_LUT[r], GAMMA_LUT[g], GAMMA_LUT[b], a);
		}

		static byte Gamma(byte c)
		{
			return GAMMA_LUT[c];
		}
	}

	namespace util
	{
		static std::mutex gw_mtx__;
		static int gw_current_index__ = 0;

		static int GetUniqueWindowIndex()
		{
			int index;
			gw_mtx__.lock();
			index = ++gw_current_index__;
			gw_mtx__.unlock();
			return index;
		}
	}

	class Series
	{
	public:
		typedef double value_type;

	public:
		Series(const std::string& label, chart::Type chartType, marker::Type markerType = marker::None)
			: label_(label),
			chart_type_(chartType),
			marker_type_(markerType),
			marker_size_({ 4,4 }),
			dimension_(chart::GetDimension(chartType)),
			enable_legend_(true),
			render_color_(color::Blue),
			dirty_(false),
			mins_(dimension_, DBL_MAX),
			maxs_(dimension_, DBL_MIN)
		{
			enable_legend_ = (chartType != chart::Elevation);
		}

		Series& operator=(Series& rhs)
		{
			label_ = rhs.label_;
			chart_type_ = rhs.chart_type_;
			marker_type_ = rhs.marker_type_;
			marker_size_ = rhs.marker_size_;
			dimension_ = rhs.dimension_;
			enable_legend_ = rhs.enable_legend_;
			values_ = rhs.values_;
			mins_ = rhs.mins_;
			maxs_ = rhs.maxs_;
			dirty_ = true;
			return *this;
		}

		Series& SetChartType(chart::Type chartType)
		{
			if (chart_type_ != chartType)
			{
				int dimension = chart::GetDimension(chartType);
				if (dimension_ == dimension)
				{
					chart_type_ = chartType;
					dirty_ = true;
				}
				else
				{
					if (values_.empty())
					{
						chart_type_ = chartType;
						dimension_ = dimension;
						dirty_ = false;
					}
					else
					{
						throw std::exception("dimension not match for the chart type");
					}
				}
			}
			return *this;
		}

		Series& SetMarkerType(marker::Type markerType)
		{
			if (marker_type_ != markerType)
			{
				marker_type_ = markerType;
				dirty_ = true;
			}
			return *this;
		}

		Series& SetMarkerSize(cv::Size size)
		{
			if (marker_size_ != size)
			{
				marker_size_ = size;
				dirty_ = true;
			}
			return *this;
		}

		Series& EnableLegend(bool enable)
		{
			if (enable_legend_ != enable)
			{
				enable_legend_ = enable;
				dirty_ = true;
			}
			return *this;
		}

		Series& SetRenderColor(Color color)
		{
			if (render_color_ != color)
			{
				render_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		Series& AddValue(value_type value)
		{
			if (dimension_ != 1)
			{
				return *this;
			}

			values_.push_back(value);
			if (maxs_[0] < mins_[0])
			{
				maxs_[0] = value;
				mins_[0] = value;
			}
			else
			{
				if (value > maxs_[0])
				{
					maxs_[0] = value;
				}
				else if (value < mins_[0])
				{
					mins_[0] = value;
				}
			}

			dirty_ = false;
			return *this;
		}

		Series& AddValues(std::vector<value_type> values)
		{
			if (dimension_ < 1)
			{
				return *this;
			}

			auto count = (values.size() / dimension_) * dimension_;
			int j = 0;
			for (int i = 0; i < count; ++i)
			{
				values_.push_back(values[i]);
				j = i % dimension_;
				if (maxs_[j] < mins_[j])
				{
					maxs_[j] = values[i];
					mins_[j] = values[i];
				}
				else
				{
					if (values[i] > maxs_[j])
					{
						maxs_[j] = values[i];
					}
					else if (values[i] < mins_[j])
					{
						mins_[j] = values[i];
					}
				}
			}

			if (count > 0)
			{
				dirty_ = true;
			}
			return *this;
		}

		Series& Clear()
		{
			if (!values_.empty())
			{
				values_.clear();
				for (int i = 0; i < dimension_; ++i)
				{
					maxs_[i] = DBL_MIN;
					mins_[i] = DBL_MAX;
				}
				dirty_ = true;
			}

			return *this;
		}

		int GetDimension() const
		{
			return dimension_;
		}

		int GetSampleCount() const
		{
			return values_.size() / dimension_;
		}

		std::vector<double> GetMax() const
		{
			return std::vector<double>(maxs_);
		}

		std::vector<double> GetMin() const
		{
			return std::vector<double>(mins_);
		}

		std::string GetLabel() const
		{
			return label_;
		}

		chart::Type GetChartType() const
		{
			return chart_type_;
		}

		marker::Type GetMarkerType() const
		{
			return marker_type_;
		}

		cv::Size GetMarkerSize() const
		{
			return marker_size_;
		}

		bool IsLegendEnabled() const
		{
			return enable_legend_;
		}

		Color GetRenderColor() const
		{
			return render_color_;
		}

		bool IsDirty() const
		{
			return dirty_;
		}

		void Draw(cv::Mat& target, int index, int division,
			double x_min, double x_max, double y_min, double y_max, double z_min, double z_max,
			double px_start, double py_start, double px_size, double py_size)
		{
			if (values_.size() < dimension_)
			{
				dirty_ = false;
				return;
			}

			auto cr = render_color_.ToScalar();

			auto xd = x_max - x_min;
			auto yd = y_max - y_min;

			switch (chart_type_)
			{
			case chart::Bar:
			{
				auto px_delta = px_size / xd;
				auto py_delta = py_size / yd;
				auto py_0 = py_delta > 10 ? 0.2 * py_delta : 2;
				int x1 = (int)(px_start + px_delta * index / (division + 1));
				int x2 = (int)(x1 + px_delta / (division + 1));
				int y1 = 0;
				int y2;
				int h = target.rows;
				auto cr1 = render_color_.Cut(90).ToScalar();
				int fbase;
				auto fface = cv::FONT_HERSHEY_SIMPLEX;
				auto fscale = 0.8;
				cv::Size fsize;
				for (auto v : values_)
				{
					y2 = (int)((v > y_min) ? (v - y_min) * py_delta : py_0);
					cv::rectangle(target, { x1,h - y1 }, { x2,h - y2 }, cr, -1);
					char szText[16] = { 0 };
					sprintf_s(szText, "%g", v);
					fsize = cv::getTextSize(szText, fface, fscale, 1, &fbase);
					cv::putText(target, szText, { (x1 + x2 - fsize.width) / 2,h - y2 - fbase }, fface, fscale, cr1, 1, cv::LINE_AA);
					x1 += px_delta;
					x2 += px_delta;
				}
			}
			break;
			case chart::Trends:
			{
				auto px_delta = px_size / xd;
				auto py_delta = py_size / yd;
				auto py_0 = py_delta > 10 ? 0.2 * py_delta : 2;
				auto x_0 = (int)(px_start + 0.5 * division * px_delta / (division + 1.1));
				int x = x_0;
				int y;
				int h = target.rows;
				std::vector<cv::Point> pts;
				for (auto v : values_)
				{
					y = (int)((v > y_min) ? (v - y_min) * py_delta : py_0);
					pts.push_back({ x,h - y });
					x += px_delta;
				}
				int r = (marker_size_.width + marker_size_.height);
				r = (r > 2 && r < 32) ? r : 4;
				DrawMarkers_(target, pts, marker_type_, render_color_.Cut(64), r, 2);
				cv::polylines(target, pts, false, cr, 1, cv::LINE_AA);

				auto cr1 = render_color_.Cut(90).ToScalar();
				int fbase;
				auto fface = cv::FONT_HERSHEY_SIMPLEX;
				auto fscale = 0.8;
				cv::Size fsize;
				x = x_0;
				for (auto v : values_)
				{
					y = (int)((v > y_min) ? (v - y_min) * py_delta : py_0);
					char szText[16] = { 0 };
					sprintf_s(szText, "%g", v);
					fsize = cv::getTextSize(szText, fface, fscale, 1, &fbase);
					cv::putText(target, szText, { x - fsize.width / 2,h - y - fbase }, fface, fscale, cr1, 1, cv::LINE_AA);
					x += px_delta;
				}
			}
			break;
			case chart::Line:
			case chart::Scatter:
			{
				int h = target.rows;
				int ci = 0;
				std::vector<cv::Point> pts;
				int x;
				int y;
				auto px_delta = px_size / xd;
				auto py_delta = py_size / yd;
				for (int i = 0; i < values_.size(); i += 2)
				{
					x = (int)(px_start + (values_[i] - x_min) * px_delta);
					y = (int)(py_start + (values_[i + 1] - y_min) * py_delta);
					pts.push_back({ x,h - y });
				}
				int r = (marker_size_.width + marker_size_.height);
				r = (r > 2 && r < 32) ? r : 4;
				
				if (chart_type_ == chart::Trends || chart_type_ == chart::Line)
				{
					DrawMarkers_(target, pts, marker_type_, render_color_.Cut(64), r, 2);
					cv::polylines(target, pts, false, cr, 1, cv::LINE_AA);
				}
				else if (chart_type_ == chart::Scatter)
				{
					DrawMarkers_(target, pts, marker_type_, render_color_, r, 2);
				}
			}
			break;
			case chart::Elevation:
			{
				int h = target.rows;
				std::vector<cv::Point> pts;
				std::vector<Color> colors;

				auto px_delta = px_size / xd;
				auto py_delta = py_size / yd;
				int x;
				int y;
				for (int i = 0; i < values_.size(); i += 3)
				{
					x = (int)(px_start + (values_[i] - x_min) * px_delta);
					y = (int)(py_start + (values_[i + 1] - y_min) * py_delta);
					pts.push_back({ x,h - y });
				}
				if (z_max > z_min)
				{
					double factor = 1.0 / (z_max - z_min);
					for (int i = 0; i < values_.size(); i += 3)
					{
						auto z = (values_[i + 2] - z_min) * factor;
						colors.push_back(render_color_.Linear(z));
					}
				}
				else
				{
					colors.resize(pts.size(), color::Transparent);
				}

				auto block_width = (int)(px_delta);
				auto block_height = (int)(py_delta);
				for (int i = 0; i < pts.size(); ++i)
				{
					cv::Rect rect({ pts[i].x - block_width / 2,pts[i].y - block_height / 2,block_width,block_height });
					cv::rectangle(target, rect, colors[i].ToScalar(), -1);
				}
			}
			break;
			default:
				break;
			}
		}

		void Dump(const std::string filename)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, filename.c_str(), "w");
			if (fp)
			{
				fprintf_s(fp, "%s\n", label_.c_str());
				fprintf_s(fp, "%d\n", chart_type_);
				fprintf_s(fp, "%d\n", marker_type_);
				fprintf_s(fp, "%d %d\n", marker_size_.width, marker_size_.height);
				fprintf_s(fp, "%d\n", dimension_);
				fprintf_s(fp, "%d\n", enable_legend_ ? 1 : 0);
				auto vec4 = render_color_.ToVec4b(); //BGRA
				fprintf_s(fp, "%d %d %d %d\n", vec4[0], vec4[1], vec4[2], vec4[3]);
				for (auto v : maxs_)
				{
					fprintf_s(fp, "%lf ", v);
				}
				for (auto v : mins_)
				{
					fprintf_s(fp, "%lf ", v);
				}
				int n = values_.size();
				fprintf_s(fp, "\n%d\n", n);
				int counter = 0;
				for (auto v : values_)
				{
					fprintf_s(fp, "%lf ", v);
					if (++counter % 32 == 0)
					{
						fprintf_s(fp, "\n");
					}
				}
				fprintf_s(fp, "\n");
			}
			fclose(fp);
		}

		Series& Load(const std::string filename)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, filename.c_str(), "r");
			if (fp == nullptr)
			{
				throw std::exception("failed to load series");
			}

			const int LEN = 256;
			char sz[LEN] = { 0 };
			fscanf_s(fp, "%s\n", sz, LEN);
			label_ = sz;
			fscanf_s(fp, "%d\n", &chart_type_);
			fscanf_s(fp, "%d\n", &marker_type_);
			fscanf_s(fp, "%d %d\n", &(marker_size_.width), &(marker_size_.height));
			fscanf_s(fp, "%d\n", &dimension_);
			int enable;
			fscanf_s(fp, "%d\n", &enable);
			enable_legend_ = enable > 0;
			int b, g, r, a;
			fscanf_s(fp, "%d %d %d %d\n", &b, &g, &r, &a);
			auto color = Color(r, g, b, a);
			render_color_ = color;
			maxs_.resize(dimension_);
			mins_.resize(dimension_);
			for (int i = 0; i < dimension_; ++i)
			{
				double v;
				fscanf_s(fp, "%lf", &v);
				maxs_[i] = v;
			}
			for (int i = 0; i < dimension_; ++i)
			{
				double v;
				fscanf_s(fp, "%lf", &v);
				mins_[i] = v;
			}
			int n;
			fscanf_s(fp, "%d\n", &n);
			if (n > 0)
			{
				values_.resize(n);
				for (int i = 0; i < n; ++i)
				{
					double v;
					fscanf_s(fp, "%lf", &v);
					values_[i] = v;
				}
			}

			fclose(fp);

			return *this;
		}

    private:
		void DrawMarkers_(cv::Mat target, std::vector<cv::Point> pts, marker::Type type, Color color, int size, int thickness)
		{
			cv::Scalar cr = color.ToScalar();
			switch (type)
			{
			case marker::Cross:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_TILTED_CROSS, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Plus:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_CROSS, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Star:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_STAR, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Circle:
			{
				for (auto pt : pts)
				{
					cv::circle(target, pt, size / 2, cr, 2, cv::LINE_8);
				}
			}
			break;
			case marker::Square:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_SQUARE, size, thickness, cv::LINE_AA);
				}
			}
			break;
			case marker::Diamond:
			{
				for (auto pt : pts)
				{
					cv::drawMarker(target, pt, cr, cv::MARKER_DIAMOND, size, thickness, cv::LINE_AA);
				}
			}
			break;
			default:
				break;
			}
		}

	protected:
		std::string label_;
		chart::Type chart_type_;
		marker::Type marker_type_;
		cv::Size marker_size_;
		int dimension_;
		bool enable_legend_;
		Color render_color_;
		std::vector<value_type> values_;
		std::vector<value_type> maxs_;
		std::vector<value_type> mins_;
		bool dirty_;
	};

	class View
	{
	public:
		View(std::string title, cv::Size size)
			:title_(title),
			size_(size),
			xlabel_(""),
			ylabel_(""),
			background_color_(color::Transparent),
			text_color_(color::Black),
			grid_color_(color::SkyBlue),
			enable_grid_(false),
			dirty_(false),
			horizontal_margin_(50),
			vertical_margin_(80)
		{
			if (size.width > 0 && size.height > 0)
			{
				cv::Size visual_size(size.width + 2 * horizontal_margin_, size.height + 2 * vertical_margin_);
				buffer_ = cv::Mat(visual_size, CV_8UC4, background_color_.ToScalar());
			}
		}

		View() : View("", { 0,0 })
		{
			//
		}

		View& operator=(View& rhs)
		{
			if (this != &rhs && rhs.IsValid())
			{
				title_ = rhs.title_;
				size_ = rhs.size_;
				xlabel_ = rhs.xlabel_;
				ylabel_ = rhs.ylabel_;
				background_color_ = rhs.background_color_;
				text_color_ = rhs.text_color_;
				enable_grid_ = rhs.enable_grid_;
				grid_color_ = rhs.grid_color_;
				horizontal_margin_ = rhs.horizontal_margin_;
				vertical_margin_ = rhs.vertical_margin_;
				buffer_ = rhs.buffer_.clone();
				series_map_ = rhs.series_map_;
				dirty_ = true;
			}
			return *this;
		}

		View& SetTitle(const std::string& title)
		{
			if (title_ != title)
			{
				title_ = title;
				dirty_ = true;
			}
			return *this;
		}

		View& SetSize(cv::Size size)
		{
			if (size_ != size)
			{
				cv::Size visual_size(size.width + 2 * horizontal_margin_, size.height + 2 * vertical_margin_);
				if (buffer_.empty())
				{
					buffer_ = cv::Mat(visual_size, CV_8UC4, color::Transparent.ToScalar());
				}
				else
				{
					auto tmp = buffer_.clone();
					cv::resize(tmp, buffer_, visual_size);
				}
				size_ = size;
				dirty_ = true;
			}
			return *this;
		}

		View& SetTextColor(Color color)
		{
			if (text_color_ != color)
			{
				text_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& SetBackgroundColor(Color color)
		{
			if (background_color_ != color)
			{
				background_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& EnableGrid(bool enable)
		{
			if (enable_grid_ != enable)
			{
				enable_grid_ = enable;
				dirty_ = true;
			}
			return *this;
		}

		View& SetGridColor(Color color)
		{
			if (grid_color_ != color)
			{
				grid_color_ = color;
				dirty_ = true;
			}
			return *this;
		}

		View& SetXLabel(const std::string& xlabel)
		{
			if (xlabel_ != xlabel)
			{
				xlabel_ = xlabel;
				dirty_ = true;
			}
			return *this;
		}

		View& SetYLabel(const std::string& ylabel)
		{
			if (ylabel_ != ylabel)
			{
				ylabel_ = ylabel;
				dirty_ = true;
			}
			return *this;
		}

		bool FindSeries(std::string label)
		{
			auto iter = series_map_.find(label);
			return (iter != series_map_.end());
		}

		View& AddSeries(Series& series)
		{
			if (!series_map_.empty())
			{
				int dim = series_map_.begin()->second.GetDimension();
				if (series.GetDimension() != dim)
				{
					throw std::exception("series dimension confilict");
				}
			}

			auto label = series.GetLabel();
			if (series_map_.find(label) == series_map_.end())
			{
				series_map_.insert({ label, series });
				dirty_ = true;
			}
			return *this;
		}

		View& Remove(const std::string label)
		{
			auto iter = series_map_.find(label);
			if (iter != series_map_.end())
			{
				series_map_.erase(label);
				dirty_ = true;
			}
			return *this;
		}

		View& Clear()
		{
			if (!series_map_.empty())
			{
				series_map_.clear();
				dirty_ = true;
			}
			return *this;
		}

		Series& SelectSeries(std::string label)
		{
			auto iter = series_map_.find(label);
			if (iter == series_map_.end())
			{
				throw std::exception("series not found");
			}
			else
			{
				return iter->second;
			}
		}

		View& Render()
		{
			if (series_map_.empty())
			{
				dirty_ = false;
			}

			if (dirty_)
			{
				//erase view with background
				//buffer_.setTo(background_color_.ToScalar());
				
				//draw y label
				if (!ylabel_.empty())
				{
					auto fface = cv::FONT_HERSHEY_TRIPLEX;
					int fbase;
					auto fsize = cv::getTextSize(ylabel_, fface, 1.0, 2, &fbase);
					int sq_size = size_.width < size_.height ? size_.width : size_.height;
					cv::Rect rect(0, size_.height / 2 + vertical_margin_ - sq_size / 2, sq_size, sq_size);
					auto mat = buffer_(rect);
					cv::Point pt(sq_size / 2 - fsize.width / 2, horizontal_margin_ - fsize.height);
					cv::putText(mat, ylabel_, pt, fface, 1.0, text_color_.ToScalar(), 2, cv::LINE_AA);
					auto rm = cv::getRotationMatrix2D({ sq_size / 2.0f,sq_size / 2.0f }, 90, 1.0);
					cv::warpAffine(mat, mat, rm, { sq_size, sq_size },cv::WARP_FILL_OUTLIERS, cv::BORDER_TRANSPARENT, color::Transparent.ToScalar());
				}

				cv::Rect roi(horizontal_margin_, vertical_margin_, size_.width, size_.height);
				auto target = buffer_(roi);

				double x_min = 0;
				double x_max = 1;
				double y_min = 0;
				double y_max = 1;
				double z_min = 0;
				double z_max = 0;

				int sample_count = 0;
				for (auto s : series_map_)
				{
					auto count1 = s.second.GetSampleCount();
					if (count1 > sample_count)
					{
						sample_count = count1;
					}
				}

				auto series_0 = series_map_.begin()->second;
				int dim = series_0.GetDimension();
				std::vector<double> mins(dim, DBL_MAX);
				std::vector<double> maxs(dim, DBL_MIN);
				
				for (auto s : series_map_)
				{
					auto mins1 = s.second.GetMin();
					auto maxs1 = s.second.GetMax();
					for (int i = 0; i < dim; ++i)
					{
						if (mins1[i] < mins[i])
						{
							mins[i] = mins1[i];
						}
						if (maxs1[i] > maxs[i])
						{
							maxs[i] = maxs1[i];
						}
					}
				}

				switch (dim)
				{
				case 1:
				{
					x_min = 1.0;
					x_max = sample_count;
					y_min = mins[0];
					y_max = maxs[0];
				}
				break;
				case 2:
				{
					x_min = mins[0];
					x_max = maxs[0];
					y_min = mins[1];
					y_max = maxs[1];
				}
				break;
				case 3:
				{
					x_min = mins[0];
					x_max = maxs[0];
					y_min = mins[1];
					y_max = maxs[1];
					z_min = mins[2];
					z_max = maxs[2];
				}
				break;
				default:
					break;
				}

				double par = 0.8125;
				double pad = (1 - par) / 2;
				double px_start = pad * size_.width;
				double py_start = pad * size_.height;
				double px_size = par * size_.width;
				double py_size = par * size_.height;

				//draw axis grids
				if (enable_grid_ && dim == 2)
				{
					auto gridLineColor = grid_color_.ToScalar();
					auto fface = cv::FONT_HERSHEY_SIMPLEX;
					int fbase;

					const int GRID_BLOCK_WIDTH = 100;
					const int GRID_BLOCK_HEIGHT = 100;
					int n_rows = target.rows / GRID_BLOCK_HEIGHT + 1;
					int n_cols = target.cols / GRID_BLOCK_WIDTH + 1;

					auto xd = (x_max - x_min) / n_cols;
					auto yd = (y_max - y_min) / n_rows;
					auto px_delta = px_size / n_cols;
					auto py_delta = py_size / n_rows;
					int x;
					int y;

					x = (int)px_start;
					y = size_.height + vertical_margin_;
					for (auto v = x_min; v < x_max + xd; v += xd)
					{
						std::ostringstream out;
						out << std::setprecision(4) << v;
						auto str = out.str();
						cv::Size fsize = getTextSize(str, fface, 0.5, 1, &fbase);
						cv::line(target, { x, 1 }, { x, size_.height - fsize.height }, gridLineColor, 1, cv::LINE_4);
						cv::Point org(x + horizontal_margin_ - fsize.width / 2, y);
						cv::putText(buffer_, str, org, fface, 0.5, text_color_.ToScalar(), 1);
						x += px_delta;
					}

					x = horizontal_margin_;
					y = size_.height - py_start;
					for (auto v = y_min; v < y_max + yd; v += yd)
					{
						std::ostringstream out;
						out << std::setprecision(4) << v;
						auto str = out.str();
						cv::Size fsize = getTextSize(str, fface, 0.5, 1, &fbase);
						cv::line(target, { fsize.width, y }, { size_.width - 1, y }, gridLineColor, 1, cv::LINE_4);
						cv::Point org(x, y + vertical_margin_ + fsize.height / 2);
						cv::putText(buffer_, str, org, fface, 0.5, text_color_.ToScalar(), 1);
						y -= py_delta;
					}
				}			

				//draw series
				int division = 0;
				for (auto s : series_map_)
				{
					if (s.second.GetChartType() == chart::Bar)
					{
						++division;
					}
				}				
				int index = 0;
				for (auto s : series_map_)
				{
					s.second.Draw(target, index, division, x_min, x_max, y_min, y_max, z_min, z_max, px_start, py_start, px_size, py_size);
					++index;					
				}

				//draw legend
				int legend_size = 6;
				int legend_x = size_.width + horizontal_margin_;
				int legend_y = vertical_margin_ + 20;
				int ci = 256 / (series_map_.size() + 1);
				for (auto s : series_map_)
				{
					if (s.second.IsLegendEnabled())
					{
						auto fface = cv::FONT_HERSHEY_COMPLEX_SMALL;
						int fbase;
						auto fsize = cv::getTextSize(s.first, fface, 1.0, 1, &fbase);
						legend_size = legend_size < fsize.height ? legend_size : fsize.height;
						cv::Point pt(legend_x - 4 * legend_size - fsize.width, legend_y);
						auto cr = s.second.GetRenderColor().ToScalar();
						cv::putText(buffer_, s.first, pt, fface, 1.0, text_color_.ToScalar(), 1, cv::LINE_AA);
						auto chartType = s.second.GetChartType();

						if (s.second.GetChartType() == chart::Line)
						{
							cv::line(buffer_,
								{ legend_x - 2 * legend_size - 4,legend_y - (fsize.height - fbase) / 2 },
								{ legend_x - legend_size,legend_y - (fsize.height - fbase) / 2 },
								cr, 2, cv::LINE_AA);
						}
						else
						{
							switch (s.second.GetMarkerType())
							{
							case marker::Cross:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_TILTED_CROSS, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Plus:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_CROSS, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Star:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_STAR, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Square:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_SQUARE, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Diamond:
								cv::drawMarker(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, cr, cv::MARKER_DIAMOND, legend_size * 2, 2, cv::LINE_AA);
								break;
							case marker::Circle:
								cv::circle(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, legend_size, cr, 2, cv::LINE_AA);
								break;
							default:
								cv::circle(buffer_, { legend_x - 2 * legend_size,legend_y - (fsize.height - fbase) / 2 }, legend_size, cr, -1, cv::LINE_AA);
								break;
							}
						}
						ci = (ci << 1) % 256;
						legend_y += 2 * fsize.height;
					}
				}

				//draw color bar
				if (series_0.GetChartType() == chart::Elevation)
				{
					auto render_color = series_0.GetRenderColor();
					int bar_width = 20;
					int bar_margin = 20;
					const int N_COLORS = 256;
					cv::Rect rect(size_.width + horizontal_margin_ + bar_margin, vertical_margin_ + bar_margin, bar_width, size_.height - 2 * bar_margin);
					cv::Mat mat({ bar_width, N_COLORS }, CV_8UC4);
					for (int i = 0; i < N_COLORS; ++i)
					{
						for (int j = 0; j < bar_width; ++j)
						{
							auto color = render_color.Linear((N_COLORS - i) / (double)N_COLORS);
							mat.at<cv::Vec4b>(i, j) = color.ToScalar();
						}
					}
					cv::Mat tmp(rect.size(), CV_8UC4);
					cv::resize(mat, tmp, { tmp.cols,tmp.rows });
					tmp.copyTo(buffer_(rect));

					auto fface = cv::FONT_HERSHEY_SIMPLEX;
					int fbase;

					char sz1[16] = { 0 };
					sprintf_s(sz1, "%g", z_min);
					auto fsize1 = cv::getTextSize(sz1, fface, 0.5, 1, &fbase);
					cv::Point pt1(rect.x + bar_width / 2 - fsize1.width / 2, rect.y + rect.height + bar_margin);
					cv::putText(buffer_, sz1, pt1, fface, 0.5, color::Black.ToScalar(), 1, cv::LINE_AA);

					char sz2[16] = { 0 };
					sprintf_s(sz2, "%g", z_max);
					auto fsize2 = cv::getTextSize(sz2, fface, 0.5, 1, &fbase);
					cv::Point pt2(rect.x + bar_width / 2 - fsize2.width / 2, rect.y - fsize2.height - fbase);
					cv::putText(buffer_, sz2, pt2, fface, 0.5, render_color.ToScalar(), 1, cv::LINE_AA);
				}

				//draw title
				if (!title_.empty())
				{
					auto fface = cv::FONT_HERSHEY_SIMPLEX;
					int fbase;
					auto fsize = cv::getTextSize(title_, fface, 1.5, 2, &fbase);
					cv::Point pt(size_.width / 2 + horizontal_margin_ - fsize.width / 2, vertical_margin_ > fsize.height ? vertical_margin_ - fsize.height : 5);
					cv::putText(buffer_, title_, pt, fface, 1.5, color::Black.ToScalar(), 2, cv::LINE_AA);
				}

				//draw x label
				if (!xlabel_.empty())
				{
					auto fface = cv::FONT_HERSHEY_TRIPLEX;
					int fbase;
					auto fsize = cv::getTextSize(xlabel_, fface, 1.0, 2, &fbase);
					cv::Point pt(size_.width / 2 + horizontal_margin_ - fsize.width / 2, size_.height + vertical_margin_ + fsize.height + 5);
					cv::putText(buffer_, xlabel_, pt, fface, 1.0, text_color_.ToScalar(), 2, cv::LINE_AA);
				}

				dirty_ = false;
			}

			return *this;
		}

		void Invalidate()
		{
			dirty_ = true;
		}

		std::string GetTitle() const
		{
			return title_;
		}

		cv::Size GetSize() const
		{
			return size_;
		}

		bool IsValid() const
		{
			return !buffer_.empty();
		}

		bool IsDirty() const
		{
			return dirty_;
		}

		cv::Mat GetBuffer() const
		{
			if (buffer_.empty())
			{
				return cv::Mat();
			}

			return buffer_.clone();
		}

		void Dump(const std::string& prefix)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, (prefix + ".000000000.vdp").c_str(), "w");
			if (fp)
			{
				fprintf_s(fp, "%s\n", title_.c_str());
				fprintf_s(fp, "%d %d\n", size_.width, size_.height);
				fprintf_s(fp, "%s\n", xlabel_.c_str());
				fprintf_s(fp, "%s\n", ylabel_.c_str());
				auto vec4 = background_color_.ToVec4b();
				fprintf_s(fp, "%d %d %d %d\n", vec4[0], vec4[1], vec4[2], vec4[3]);
				vec4 = text_color_.ToVec4b();
				fprintf_s(fp, "%d %d %d %d\n", vec4[0], vec4[1], vec4[2], vec4[3]);
				vec4 = grid_color_.ToVec4b();
				fprintf_s(fp, "%d %d %d %d\n", vec4[0], vec4[1], vec4[2], vec4[3]);
				fprintf_s(fp, "%d\n", enable_grid_ ? 1 : 0);
				fprintf_s(fp, "%d %d\n", horizontal_margin_, vertical_margin_);
				int n = series_map_.size();
				fprintf_s(fp, "%d\n", n);
				int index = 0;
				for (auto s : series_map_)
				{
					char sz[32] = { 0 };
					sprintf_s(sz, ".%09d.sdp", ++index);
					auto filename = prefix + sz;
					s.second.Dump(filename);
					fprintf_s(fp, "%s\n", filename.c_str());
				}
				fclose(fp);
			}
		}

		View& Load(const std::string prefix)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, (prefix + ".000000000.vdp").c_str(), "r");
			if (fp)
			{
				const int LEN = 256;
				char sz[LEN] = { 0 };
				fscanf_s(fp, "%s\n", sz, LEN);
				title_ = sz;
				int w;
				int h;
				fscanf_s(fp, "%d %d\n", &w, &h);
				SetSize({ w,h });
				char sz1[LEN] = { 0 };
				fscanf_s(fp, "%s\n", sz1, LEN);
				xlabel_ = sz1;
				char sz2[LEN] = { 0 };
				fscanf_s(fp, "%s\n", sz2, LEN);
				ylabel_ = sz2;
				int r, g, b, a;
				fscanf_s(fp, "%d %d %d %d\n", &b, &g, &r, &a);
				auto color1 = Color(r, g, b, a);
				background_color_ = color1;
				fscanf_s(fp, "%d %d %d %d\n", &b, &g, &r, &a);
				auto color2 = Color(r, g, b, a);
				text_color_ = color2;
				fscanf_s(fp, "%d %d %d %d\n", &b, &g, &r, &a);
				auto color3 = Color(r, g, b, a);
				grid_color_ = color3;
				int enable;
				fscanf_s(fp, "%d\n", &enable);
				enable_grid_ = enable > 0;
				fscanf_s(fp, "%d %d\n", &horizontal_margin_, &vertical_margin_);
				int n;
				fscanf_s(fp, "%d\n", &n);
				Clear();
				for (int i = 0; i < n; ++i)
				{
					char szf[LEN] = { 0 };
					fscanf_s(fp, "%s\n", szf, LEN);
					Series s("", chart::Line);
					s.Load(szf);
					AddSeries(s);
				}
				fclose(fp);
				return *this;
			}

			throw std::exception("failed to load view");
		}

	private:
		static double CalcSnap_(double value)
		{
			auto v1 = pow(10, floor(log10(value)));
			auto v2 = pow(10, floor(log10(value / 2))) * 2;
			auto v3 = pow(10, floor(log10(value / 5))) * 5;
			return std::max({ v1,v2,v3 });
		}

	protected:
		std::string title_;
		cv::Size size_;
		std::string xlabel_;
		std::string ylabel_;
		Color background_color_;
		Color text_color_;
		Color grid_color_;
		bool enable_grid_;
		bool dirty_;
		cv::Mat buffer_;
		int horizontal_margin_;
		int vertical_margin_;
		std::map<std::string, Series> series_map_;
	};

	class Figure
	{
	public:
		Figure()
			: horizontal_count_(1),
			vertical_count_(1),
			horizontal_margin_(20),
			vertical_margin_(20),
			figure_size_({ 800,800 }),
			buffer_(800, 800, CV_8UC4, color::Gray.ToScalar())
		{
			int index = util::GetUniqueWindowIndex();
			char sz[16] = { 0 };
			sprintf_s(sz, "fig-%09d", index);
			figure_name_ = sz;
			views_.push_back(View());
		}

		~Figure()
		{
			cv::destroyWindow(figure_name_);
		}

		Figure& SetSize(cv::Size size)
		{
			if (figure_size_ != size)
			{
				figure_size_ = size;
				auto tmp = cv::Mat(figure_size_, CV_8UC4, Color(200, 200, 200, 128).ToScalar());
				cv::resize(buffer_, tmp, figure_size_, 0, 0, cv::INTER_NEAREST);
				buffer_ = tmp;
			}
			return *this;
		}

		Figure& SetLayout(int rows, int cols)
		{
			int count_ = horizontal_count_ * vertical_count_;
			int count = rows * cols;
			if (count_ != count)
			{
				views_.resize(count, View("", { 800,800 }));
			}

			horizontal_count_ = cols;
			vertical_count_ = rows;
			return *this;
		}

		Figure& SetView(View& view, int hpos, int vpos)
		{
			int index = (hpos - 1) * horizontal_count_ + vpos - 1;
			if (index < 0 || index >= views_.size())
			{
				throw std::out_of_range("view index out of range");
			}
			views_[index] = view;
			return *this;
		}

		View& SelectView(int hpos, int vpos)
		{
			int index = (hpos - 1) * horizontal_count_ + vpos - 1;
			if (index < 0 || index >= views_.size())
			{
				throw std::out_of_range("view index out of range");
			}
			else
			{
				return views_[index];
			}
		}

		void Show(bool waitKey = true)
		{
			Render_();
			cv::namedWindow(figure_name_, cv::WINDOW_AUTOSIZE);
			const int SCW = 1920;
			const int SCH = 800;
			int x = (SCW - figure_size_.width) / 2;
			int y = (SCH - figure_size_.height) / 2;
			x = (x > 0 && x < 200) ? x : 10;
			y = (y > 0 && y < 200) ? y : 10;
			cv::moveWindow(figure_name_, x, y);
			cv::imshow(figure_name_, buffer_);
			if (waitKey)
			{
				cv::waitKey();
			}
		}

		void Close()
		{
			cv::destroyWindow(figure_name_);
		}

		void Save(const std::string& filename, bool saveSubViews = false)
		{
			Render_();
			try
			{
				cv::imwrite(filename, buffer_);
				if (saveSubViews && !render_results_.empty())
				{
					auto index = filename.find_last_of('.');
					std::string prefix = filename.substr(0, index);
					std::string ext = filename.substr(index);
					for (auto result : render_results_)
					{
						auto filename_1 = prefix + "[" + result.first + "]" + ext;
						cv::imwrite(filename_1, result.second);
					}
				}
			}
			catch (...)
			{

			}
		}

		void Dump(const std::string& folder, const std::string alias)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, (folder + alias + ".00000000000000.fdp").c_str(), "w");
			if (fp)
			{
				fprintf_s(fp, "%s\n", figure_name_.c_str());
				fprintf_s(fp, "%d %d\n", vertical_count_, horizontal_count_);
				fprintf_s(fp, "%d %d\n", figure_size_.width, figure_size_.height);
				fprintf_s(fp, "%d %d\n", horizontal_margin_, vertical_margin_);
				for (int i = 1; i <= vertical_count_; ++i)
				{
					for (int j = 1; j <= horizontal_count_; ++j)
					{
						auto v = SelectView(i, j);
						char sz[32] = { 0 };
						sprintf_s(sz, ".%02d-%02d", i, j);
						auto prefix = folder + alias + sz;
						v.Dump(prefix);
						fprintf_s(fp, "%s\n", prefix.c_str());
					}
				}
				fclose(fp);
			}
		}

		void Load(const std::string& folder, const std::string alias)
		{
			FILE* fp = nullptr;
			fopen_s(&fp, (folder + alias + ".00000000000000.fdp").c_str(), "r");
			if (fp == nullptr)
			{
				throw std::exception("load figure failed");
			}

			const int LEN = 256;
			char sz[LEN] = { 0 };
			fscanf_s(fp, "%s\n", sz, LEN);
			figure_name_ = sz;
			int rows;
			int cols;
			fscanf_s(fp, "%d %d\n", &rows, &cols);
			SetLayout(rows, cols);
			fscanf_s(fp, "%d %d\n", &cols, &rows);
			SetSize({ cols,rows });
			fscanf_s(fp, "%d %d\n", &horizontal_margin_, &vertical_margin_);
			for (int i = 1; i <= vertical_count_; ++i)
			{
				for (int j = 1; j <= horizontal_count_; ++j)
				{
					View v;
					char sz[32] = { 0 };
					sprintf_s(sz, ".%02d-%02d", i, j);
					auto prefix = folder + alias + sz;
					v.Load(prefix);
					SetView(v, i, j);
					SelectView(i, j).Invalidate();
				}
			}
			fclose(fp);
		}

	private:
		void Render_()
		{
			bool dirty_ = false;
			for (auto view : views_)
			{
				if (view.IsDirty())
				{
					dirty_ = true;
					break;
				}
			}

			if (dirty_)
			{
				auto res_width = figure_size_.width - (horizontal_count_ + 1) * horizontal_margin_;
				auto res_height = figure_size_.height - (vertical_count_ + 1) * vertical_margin_;
				cv::Size size =
				{
					res_width / horizontal_count_,
					res_height / vertical_count_
				};

				cv::Rect roi(horizontal_margin_, 0, size.width, size.height);

				int index = 0;
				for (int i = 0; i < horizontal_count_; ++i)
				{
					roi.y = vertical_margin_;
					for (int j = 0; j < vertical_count_; ++j)
					{
						auto vbuf = views_[index].Render().GetBuffer();
						if (!vbuf.empty())
						{
							char sz[8] = { 0 };
							sprintf_s(sz, "%02d-%02d", j + 1, i + 1);
							if (render_results_.find(sz) == render_results_.end())
							{
								render_results_.insert({ sz, vbuf });
							}
							else
							{
								render_results_[sz] = vbuf;
							}
							auto m = buffer_(roi);
							if (vbuf.rows != size.height || vbuf.cols != size.width)
							{
								cv::Mat tmp(size, CV_8UC4);
								cv::resize(vbuf, tmp, size, cv::INTER_NEAREST);
								tmp.copyTo(m);
								tmp.release();
							}
							else
							{
								vbuf.copyTo(m);
							}
						}
						++index;
						roi.y += (size.height + vertical_margin_);
					}
					roi.x += (size.width + horizontal_margin_);
				}

				dirty_ = false;
			}
		}

	private:
		std::vector<View> views_;
		std::map<std::string, cv::Mat> render_results_;
		std::string figure_name_;
		cv::Size figure_size_;
		int horizontal_count_;
		int vertical_count_;
		int horizontal_margin_;
		int vertical_margin_;
		cv::Mat buffer_;
	};
}

#endif  // CVPLOT_H
