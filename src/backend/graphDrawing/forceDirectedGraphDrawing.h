#include "../../core/graph.h"
#include <unsupported/Eigen/NonLinearOptimization>

std::vector<double> force_directed_graph_drawing(const ArticleGraph& event_network, const std::vector<double>& x_positions);

struct Functor
{
	using InputType = Eigen::VectorXd;
	using ValueType = Eigen::VectorXd;
	using Matrix = Eigen::MatrixXd;
	using Scalar = double;
	using JacobianType = Matrix;

	enum {
		InputsAtCompileTime = Eigen::Dynamic,
		ValuesAtCompileTime = Eigen::Dynamic
	};

	const int m_inputs, m_values;

	Functor(int inputs, int values);

	int inputs() const;
	int values() const;
};

class ForceDirectedFunctor : public Functor
{
	private:
		static constexpr double _zeta = 1;

		static constexpr double _alpha = 1;
		static constexpr double _beta = 1;
		static constexpr double _gamma = 1;

		
		const ArticleGraph& _event_network;
		const std::vector<double>& _x_positions;
		const std::size_t _n;

	public:
		ForceDirectedFunctor(const ArticleGraph& event_network, const std::vector<double>& x_positions);

		double r(double s) const;
		double ds_r(double s) const;
		double norm(double x1, double x2, double y1, double y2) const;
		double dy1_norm(double x1, double x2, double y1, double y2) const;
		double dy2_norm(double x1, double x2, double y1, double y2) const;

		int operator()(const InputType& y, ValueType& fvec) const;
		int df(const InputType& y, Matrix& fjac) const;
};
