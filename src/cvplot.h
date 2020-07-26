#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef byte
typedef unsigned char byte;
#endif

namespace cvplot
{
	namespace chart
	{
		typedef int Type;

		static const Type Bar = 0;
		static const Type Line = 1;
		static const Type Scatter = 2;
		static const Type Elevation = 3;

		static int GetDimensions(Type type)
		{
			int dim = 1;

			switch (type)
			{
			case Bar:
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
		static const Type Circle = 3;
	}

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

		Color() : Color(0,0,0)
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
				r_ = rhs.a_;
				g_ = rhs.g_;
				b_ = rhs.g_;
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

		Color Mix(byte a)
		{
			return Color(r_, g_, b_, a);
		}

		cv::Scalar ToScalar() const
		{
			return cv::Scalar(b_, g_, r_, a_);
		}

	private:
		byte r_, g_, b_, a_;
	};

	namespace color
	{
		static const Color Transparent = Color(255, 255, 255, 0);
		static const Color Black = Color(0, 0, 0);
		static const Color Gray = Color(128, 128, 128);
		static const Color White = Color(255, 255, 255);
		static const Color Red = Color(255, 0, 0);
		static const Color Green = Color(0, 255, 0);		
		static const Color Blue = Color(0, 0, 255);

		typedef int Scheme;

		static const Scheme GradientGray = 0;
		static const Scheme GradientRed = 1;
		static const Scheme GradientGreen = 2;
		static const Scheme GradientBlue = 3;

		static Color Index(int index, Scheme scheme)
		{
			byte r = 0;
			byte g = 0;
			byte b = 0;

			switch (scheme)
			{
			case GradientRed:
				r = (byte)(index > 128 ? index : 255 - index);
				g = (byte)(index > 64 ? 64 : 64 - index);
				b = (byte)(index > 64 ? 64 : 64 - index);
				break;
			case GradientGreen:
				r = (byte)(index > 64 ? 64 : 64 - index);
				g = (byte)(index > 128 ? index : 255 - index);
				b = (byte)(index > 64 ? 64 : 64 - index);
				break;
			case GradientBlue:
				r = (byte)(index > 64 ? 64 : 64 - index);
				g = (byte)(index > 64 ? 64 : 64 - index);
				b = (byte)(index > 128 ? index : 255 - index);
				break;
			default:
				r = (byte)index;
				g = (byte)index;
				b = (byte)index;
				break;
			}

			return Color(r, g, b);
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
			dimension_(chart::GetDimensions(chartType)),
			enable_legend_(false),
			mins_(dimension_, DBL_MAX),
			maxs_(dimension_, DBL_MIN)
		{
			//
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
			return *this;
		}

		int GetDimension() const
		{
			return dimension_;
		}

		int GetCount() const
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

		std::string GetName() const
		{
			return label_;
		}

		void Draw(cv::Mat& buffer, int index, int division,
			double x_min, double x_max, double x_delta, 
			double y_min, double y_max, double y_delta)
		{
			switch (chart_type_)
			{
			case chart::Bar:
			{
				int x1 = (int)(x_min + x_delta * index + x_delta);
				int x2 = (int)(x1 + x_delta);
				int xd = (int)(x_delta * division);
				int y1 = 0;
				int y2;
				int h = buffer.rows;
				int ci = 10;
				for (auto v : values_)
				{
					y2 = (int)(v * y_delta);
					cv::rectangle(buffer, { x1,h - y1 }, { x2,h - y2 }, color::Index(index * ci, color::GradientBlue).ToScalar(), -1);
					char sz[16] = { 0 };
					sprintf_s(sz, "%g", v);
					int b;
					auto f = cv::getTextSize(sz, cv::FONT_HERSHEY_SIMPLEX, 1.0, 1, &b);
					cv::putText(buffer, sz, { x1 + ((int)x_delta - f.width) / 2,h - y2 - 5 }, cv::FONT_HERSHEY_SIMPLEX, 0.5, Color(255, 0, 255).ToScalar());
					x1 += xd;
					x2 += xd;
					ci += 2;
				}
			}
			break;
			case chart::Line:
			{
				int h = buffer.rows;
				int ci = 0;
				std::vector<cv::Point> pts;
				for (int i = 0; i < values_.size(); i += 2)
				{
					int x = (int)((values_[i] - x_min) * x_delta);
					int y = (int)((values_[i + 1] - y_min) * y_delta);
					pts.push_back({ x,h - y });
				}
				switch (marker_type_)
				{
				case marker::Circle:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::circle(buffer, pt, r, color::Green.ToScalar(), 2, cv::LINE_8);
					}
				}
				break;
				case marker::Cross:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::line(buffer, { pt.x - r,pt.y - r }, { pt.x + r,pt.y + r }, color::Green.ToScalar(), 2, cv::LINE_AA);
						cv::line(buffer, { pt.x - r,pt.y + r }, { pt.x + r,pt.y - r }, color::Green.ToScalar(), 2, cv::LINE_AA);
					}
				}
				break;
				case marker::Plus:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::line(buffer, { pt.x - r,pt.y }, { pt.x + r,pt.y }, color::Green.ToScalar(), 2, cv::LINE_AA);
						cv::line(buffer, { pt.x,pt.y - r }, { pt.x,pt.y + r }, color::Green.ToScalar(), 2, cv::LINE_AA);
					}
				}
				break;
				default:
					break;
				}
				cv::polylines(buffer, pts, false, color::Blue.ToScalar(), 1, cv::LINE_AA);
			}
			break;
			case chart::Scatter:
			{
				int h = buffer.rows;
				int ci = 0;
				std::vector<cv::Point> pts;
				for (int i = 0; i < values_.size(); i += 2)
				{
					int x = (int)((values_[i] - x_min) * x_delta);
					int y = (int)((values_[i + 1] - y_min) * y_delta);
					pts.push_back({ x,h - y });
				}
				switch (marker_type_)
				{
				case marker::Circle:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::circle(buffer, pt, r, color::Red.ToScalar(), 2, cv::LINE_8);
					}
				}
				break;
				case marker::Cross:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::line(buffer, { pt.x - r,pt.y - r }, { pt.x + r,pt.y + r }, color::Red.ToScalar(), 2, cv::LINE_AA);
						cv::line(buffer, { pt.x - r,pt.y + r }, { pt.x + r,pt.y - r }, color::Red.ToScalar(), 2, cv::LINE_AA);
					}
				}
				break;
				case marker::Plus:
				{
					int r = (marker_size_.width + marker_size_.height) / 2;
					r = (r > 2 && r < 8) ? r : 4;
					for (auto pt : pts)
					{
						cv::line(buffer, { pt.x - r,pt.y }, { pt.x + r,pt.y }, color::Red.ToScalar(), 2, cv::LINE_AA);
						cv::line(buffer, { pt.x,pt.y - r }, { pt.x,pt.y + r }, color::Red.ToScalar(), 2, cv::LINE_AA);
					}
				}
				break;
				default:
					break;
				}
			}
			break;
			case chart::Elevation:
			{
				int h = buffer.rows;
				int ci = 0;
				std::vector<cv::Point> pts;
				std::vector<double> elevations;
				for (int i = 0; i < values_.size(); i += 3)
				{
					int x = (int)((values_[i] - x_min) * x_delta);
					int y = (int)((values_[i + 1] - y_min) * y_delta);
					elevations.push_back(values_[i + 2]);
					pts.push_back({ x,h - y });
				}

				auto mw = (int)(x_delta);
				auto mh = (int)(y_delta);
				for (int i = 0; i < pts.size(); ++i)
				{
					cv::Rect rect({ pts[i].x - mw / 2,pts[i].y - mh / 2,mw,mh });
					cv::rectangle(buffer, rect, color::Index(elevations[i], color::GradientGray).ToScalar(), -1);
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
		std::vector<value_type> values_;
		std::vector<value_type> maxs_;
		std::vector<value_type> mins_;
	};

	class View
	{
	public:
		View(cv::Size size)
			:title_(""),
			size_(size),
			xlabel_(""),
			ylabel_(""),
			bg_color_(color::Transparent),
			fg_color_(color::Black),
			dirty_(false)
		{
			if (size.width > 0 && size.height > 0)
			{
				buffer_ = cv::Mat(size, CV_8UC4, bg_color_.ToScalar());
			}
		}

		View() : View({ 0,0 })
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
				bg_color_ = rhs.bg_color_;
				fg_color_ = rhs.fg_color_;
				buffer_ = rhs.buffer_;
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

		View& SetSize(const cv::Size& size)
		{
			if (size_ != size)
			{
				if (buffer_.empty())
				{
					buffer_ = cv::Mat(size, CV_8UC4,color::Transparent.ToScalar());
				}
				else
				{
					auto tmp = buffer_.clone();
					cv::resize(tmp, buffer_, size);
				}
				size_ = size;
				dirty_ = true;
			}
			return *this;
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
			auto name = series.GetName();
			if (series_map_.find(name) == series_map_.end())
			{
				series_map_.insert({ name, series });
				dirty_ = true;
			}
			return *this;
		}

		View& Clear()
		{
			series_map_.clear();
		}

		View& Render()
		{
			if (series_map_.empty())
			{
				dirty_ = false;
			}

			if (dirty_)
			{
				//cv::rectangle(buffer_, { 0, 0 }, { size_.width - 1, size_.height - 1 }, color::Black.ToScalar(), 1, cv::LINE_AA);

				double x_min, x_max, x_delta;
				double y_min, y_max, y_delta;

				int dim = series_map_.begin()->second.GetDimension();
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
					int count = 0;
					for (auto s : series_map_)
					{
						auto count1 = s.second.GetCount();
						if (count1 > count)
						{
							count = count1;
						}
					}
					x_min = 0.0;
					x_max = 2.0 * count;
					y_min = mins[0];
					y_max = maxs[0];					
				}
				break;
				case 2:
				case 3:
				{
					x_min = mins[0];
					x_max = maxs[0];					
					y_min = mins[1];
					y_max = maxs[1];					
				}
				break;
				default:
					x_min = 0;
					x_max = 1;
					x_delta = 1;
					y_min = 0;
					y_max = 1;
					y_delta = 1;
					break;
				}

				x_delta = 0.8125 * size_.width / (x_max - x_min);
				y_delta = 0.8125 * size_.height / (y_max - y_min);
				x_min = (11.0 * x_min - x_max) / 10.0;
				x_max = (11.0 * x_max - x_min) / 10.0;
				y_min = (11.0 * y_min - y_max) / 10.0;
				y_max = (11.0 * y_max - y_min) / 10.0;

				if (dim == 2)
				{
					int n_rows = buffer_.rows / y_delta + 1;
					for (auto i = 0; i < n_rows; ++i)
					{
						auto y = (int)(i * y_delta);
						cv::line(buffer_, { 1, buffer_.rows - y },
							{ buffer_.cols - 1, buffer_.rows - y },
							Color(180,180,192).ToScalar(), 1, cv::LINE_4);
					}

					int n_cols = buffer_.cols / x_delta + 1;
					for (auto i = 0; i < n_cols; ++i)
					{
						auto x = (int)(i * x_delta);
						cv::line(buffer_, { x, 1 },
							{ x, buffer_.rows - 1 },
							Color(180, 180, 192).ToScalar(), 1, cv::LINE_4);
					}
				}

				int index = 0;
				int division = series_map_.size() + 1;
				for (auto s : series_map_)
				{
					s.second.Draw(buffer_, index, division, x_min, x_max, x_delta, y_min, y_max, y_delta);
					++index;
				}

				index = 0;
				cv::Point pt(buffer_.cols - 200, 50);
				for (auto s : series_map_)
				{
					cv::putText(buffer_, s.first, pt, cv::FONT_HERSHEY_TRIPLEX, 1.0, Color(255, 128, 0).ToScalar());
					pt.y += 50;
				}
				dirty_ = false;
			}

			return *this;
		}

		void Invalidate()
		{
			dirty_ = true;
		}

		std::string GetName() const
		{
			return title_;
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

		private:
			static double CalcSnap(double value)
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
		Color bg_color_;
		Color fg_color_;
		bool dirty_;
		cv::Mat buffer_;
		std::map<std::string, Series> series_map_;
	};

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
				auto tmp = cv::Mat(figure_size_, CV_8UC4, Color(200,200,200,128).ToScalar());
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
				views_.resize(count, View({ 800,800 }));
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
		std::map<std::string,cv::Mat> render_results_;
		std::string figure_name_;
		cv::Size figure_size_;
		int horizontal_count_;
		int vertical_count_;
		int horizontal_margin_;
		int vertical_margin_;
		cv::Mat buffer_;
	};
}

