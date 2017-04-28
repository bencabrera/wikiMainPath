#include "forceDirectedGraphDrawing.h"
#include <random>
#include <iostream>

std::string flag_to_string(int flag)
{
	static std::map<int, std::string> m = {
		{ -2, "NotStarted" },
		{ -1, "Running" },
		{ 0, "ImproperInputParameters" },
		{ 1, "RelativeReductionTooSmall" },
		{ 2, "RelativeErrorTooSmall" },
		{ 3, "RelativeErrorAndReductionTooSmall" },
		{ 4, "CosinusTooSmall" },
		{ 5, "TooManyFunctionEvaluation" },
		{ 6, "FtolTooSmall" },
		{ 7, "XtolTooSmall" },
		{ 8, "GtolTooSmall" },
		{ 9, "UserAsked" }
	};

	return m.at(flag);
}

Functor::Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}
int Functor::inputs() const { return m_inputs; }
int Functor::values() const { return m_values; }


ForceDirectedFunctor::ForceDirectedFunctor(const ArticleGraph& event_network, const std::vector<double>& x_positions)
	:Functor(x_positions.size(), x_positions.size() + 2 * x_positions.size() * (x_positions.size() - 1)),
	_event_network(event_network),
	_x_positions(x_positions),
	_n(x_positions.size())
{}

double ForceDirectedFunctor::r(double s) const
{
	return std::pow(s, -_alpha);
}

double ForceDirectedFunctor::ds_r(double s) const
{
	return std::pow(s, -_alpha-1);
}

double ForceDirectedFunctor::norm(double x1, double x2, double y1, double y2) const
{
	return std::sqrt(std::pow(x1-x2,2) + std::pow(y1-y2,2));
}

double ForceDirectedFunctor::dy1_norm(double x1, double x2, double y1, double y2) const
{
	return -(y1 - y2) / norm(x1,x2,y1,y2);
}

double ForceDirectedFunctor::dy2_norm(double x1, double x2, double y1, double y2) const
{
	return (y1 - y2) / norm(x1,x2,y1,y2);
}

int ForceDirectedFunctor::operator()(const InputType& y, ValueType& fvec) const
{
	fvec.setZero();

	// std::cout << "y: " << y << std::endl;
	for(std::size_t i = 0; i < _n; i++)
	{
		// fvec(i) = r(y(i)) + r(1-y(i));
		fvec(i) = _alpha * (r(y(i)) + r(1-y(i))+ 25 * std::pow(y(i)-0.5,2));
		// fvec(i) = (y(i) - 0.5) * (y(i) - 0.5);
	}

	std::size_t cur_pos = _n;
	for(std::size_t i = 0; i < _n; i++)
		for(std::size_t j = 0; j < _n; j++)
			if(i != j)
			{
				if(boost::edge(i, j, _event_network).second)
					fvec(cur_pos) = _beta * std::pow(y(i) - y(j), 2.0);

				cur_pos++;
			}

	for(std::size_t i = 0; i < _n; i++)
		for(std::size_t j = 0; j < _n; j++)
			if(i != j)
			{
				fvec(cur_pos) = _gamma * r(norm(_x_positions[i],_x_positions[j],y(i),y(j)));

				cur_pos++;
			}
	// std::cout << "fvec: " << std::endl << fvec << std::endl;

	return 0;
}

int ForceDirectedFunctor::df(const InputType& y, Matrix& fjac) const
{
	fjac.setZero();

	// std::cout << "y: " << y << std::endl;
	for(std::size_t i = 0; i < _n; i++)
	{
		fjac(i,i) = - ds_r(y(i)) + ds_r(1-y(i)) + 25 * 2 * (y(i) - 0.5);
	}

	std::size_t cur_pos = _n;
	for(std::size_t i = 0; i < _n; i++)
		for(std::size_t j = 0; j < _n; j++)
			if(i != j)
			{
				if(boost::edge(i, j, _event_network).second)
				{
					fjac(cur_pos, i) = _beta * 2 * (y(i) - y(j));
					fjac(cur_pos, j) = _beta * -2 * (y(i) - y(j));
				}

				cur_pos++;
			}

	// fvec(cur_pos) = _gamma * r(norm(_x_positions[i],_x_positions[j],y(i),y(j)));
	for(std::size_t i = 0; i < _n; i++)
		for(std::size_t j = 0; j < _n; j++)
			if(i != j)
			{
				fjac(cur_pos, i) = 
					_gamma 
					* ds_r(norm(_x_positions[i],_x_positions[j],y(i),y(j))) 
					* dy1_norm(_x_positions[i],_x_positions[j],y(i),y(j));

				fjac(cur_pos, j) = 
					_gamma 
					* ds_r(norm(_x_positions[i],_x_positions[j],y(i),y(j))) 
					* dy2_norm(_x_positions[i],_x_positions[j],y(i),y(j));

				cur_pos++;
			}

	// std::cout << "fjac: " << fjac << std::endl;

	return 0;
}

std::vector<double> force_directed_graph_drawing(const ArticleGraph& event_network, const std::vector<double>& x_positions)
{
	ForceDirectedFunctor f(event_network, x_positions);
	Eigen::NumericalDiff<ForceDirectedFunctor> f_numerical_diff(f);



	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<ForceDirectedFunctor>, double> lm_solver(f_numerical_diff);
	// Eigen::LevenbergMarquardt<ForceDirectedFunctor, double> lm_solver(f);

	ForceDirectedFunctor::InputType y(x_positions.size());

	std::random_device rd;
	std::mt19937 rand_gen(rd());
	std::uniform_real_distribution<double> unif_0_1;
	for(int i = 0; i < y.size(); i++)
		y(i) = unif_0_1(rand_gen);

	int flag = lm_solver.minimize(y);
	std::cout << "FLAG: " << flag_to_string(flag) << std::endl;

	std::vector<double> rtn;
	rtn.resize(y.size());
	Eigen::VectorXd::Map(&rtn[0], y.size()) = y;

	return rtn;
}
