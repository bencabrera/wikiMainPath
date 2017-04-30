#include "forceDirectedFunctors.h"


namespace WikiMainPath {
	namespace GraphDrawing {

		// --- helpers ---
		
		constexpr double _zeta = 1;
		constexpr double _alpha = 1;
		constexpr double _beta = 1;
		constexpr double _gamma = 1;


		double r(double s)
		{
			return std::pow(s, -_alpha);
		}

		double ds_r(double s)
		{
			return std::pow(s, -_alpha-1);
		}

		double norm(double x1, double x2, double y1, double y2)
		{
			return std::sqrt(std::pow(x1-x2,2) + std::pow(y1-y2,2));
		}

		double dy1_norm(double x1, double x2, double y1, double y2)
		{
			return -(y1 - y2) / norm(x1,x2,y1,y2);
		}

		double dy2_norm(double x1, double x2, double y1, double y2)
		{
			return (y1 - y2) / norm(x1,x2,y1,y2);
		}

		int compute_n_values(const ArticleGraph& event_network, const std::vector<double>& x_positions)
		{
			int rtn = 0;
			rtn += x_positions.size(); 								// boundary conditions
			rtn += boost::num_edges(event_network); 				// edge attraction conditions
			rtn += x_positions.size() * (x_positions.size() - 1); 	// repulsion conditions
			return rtn;
		}


		// ---------------------------------




		template<typename TJacobianType>
			Functor<TJacobianType>::Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}
		template<typename TJacobianType>
			int Functor<TJacobianType>::inputs() const { return m_inputs; }
		template<typename TJacobianType>
			int Functor<TJacobianType>::values() const { return m_values; }

		template Functor<Eigen::MatrixXd>::Functor(int inputs, int values);
		template int Functor<Eigen::MatrixXd>::inputs() const;
		template int Functor<Eigen::MatrixXd>::values() const;

		template Functor<SparseMatrix>::Functor(int inputs, int values);
		template int Functor<SparseMatrix>::inputs() const;
		template int Functor<SparseMatrix>::values() const;


		// --- non-sparse jacobian ---
		ForceDirectedFunctor::ForceDirectedFunctor(const ArticleGraph& event_network, const std::vector<double>& x_positions)
			:Functor(x_positions.size(), compute_n_values(event_network,x_positions)),
			_event_network(event_network),
			_x_positions(x_positions),
			_n(x_positions.size())
		{}


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
			for (auto e : boost::make_iterator_range(boost::edges(_event_network))) {
				auto i = boost::source(e,_event_network);	
				auto j = boost::target(e,_event_network);	
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

		int ForceDirectedFunctor::df(const InputType& y, JacobianType& fjac) const
		{
			fjac.setZero();

			// std::cout << "y: " << y << std::endl;
			for(std::size_t i = 0; i < _n; i++)
			{
				fjac(i,i) = - ds_r(y(i)) + ds_r(1-y(i)) + 25 * 2 * (y(i) - 0.5);
			}

			std::size_t cur_pos = _n;
			for (auto e : boost::make_iterator_range(boost::edges(_event_network))) {
				auto i = boost::source(e,_event_network);	
				auto j = boost::target(e,_event_network);	
				fjac(cur_pos, i) = _beta * 2 * (y(i) - y(j));
				fjac(cur_pos, j) = _beta * -2 * (y(i) - y(j));
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



		// --- sparse jacobian ---
		
		ForceDirectedSparseFunctor::ForceDirectedSparseFunctor(const ArticleGraph& event_network, const std::vector<double>& x_positions)
			:Functor(x_positions.size(), compute_n_values(event_network,x_positions)),
			_event_network(event_network),
			_x_positions(x_positions),
			_n(x_positions.size())
		{}


		int ForceDirectedSparseFunctor::operator()(const InputType& y, ValueType& fvec) const
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
			for (auto e : boost::make_iterator_range(boost::edges(_event_network))) {
				auto i = boost::source(e,_event_network);	
				auto j = boost::target(e,_event_network);	
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

		int ForceDirectedSparseFunctor::df(const InputType& y, JacobianType& fjac) const
		{
			// std::cout << "y: " << y << std::endl;
			for(std::size_t i = 0; i < _n; i++)
			{
				fjac.insert(i,i) = - ds_r(y(i)) + ds_r(1-y(i)) + 25 * 2 * (y(i) - 0.5);
			}

			std::size_t cur_pos = _n;
			for (auto e : boost::make_iterator_range(boost::edges(_event_network))) {
				auto i = boost::source(e,_event_network);	
				auto j = boost::target(e,_event_network);	
				fjac.insert(cur_pos, i) = _beta * 2 * (y(i) - y(j));
				fjac.insert(cur_pos, j) = _beta * -2 * (y(i) - y(j));
				cur_pos++;
			}

			// fvec(cur_pos) = _gamma * r(norm(_x_positions[i],_x_positions[j],y(i),y(j)));
			for(std::size_t i = 0; i < _n; i++)
				for(std::size_t j = 0; j < _n; j++)
					if(i != j)
					{
						fjac.insert(cur_pos, i) = 
							_gamma 
							* ds_r(norm(_x_positions[i],_x_positions[j],y(i),y(j))) 
							* dy1_norm(_x_positions[i],_x_positions[j],y(i),y(j));

						fjac.insert(cur_pos, j) = 
							_gamma 
							* ds_r(norm(_x_positions[i],_x_positions[j],y(i),y(j))) 
							* dy2_norm(_x_positions[i],_x_positions[j],y(i),y(j));

						cur_pos++;
					}

			// std::cout << "fjac: " << fjac << std::endl;

			return 0;
		}
	}
}
