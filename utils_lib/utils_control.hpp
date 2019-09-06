#ifndef UTILS_CONTROL_HPP
#define UTILS_CONTROL_HPP

#include "utils_math.hpp"


namespace utils::control {
    /**
     *  Small, easy to use PID implementation with advanced controller capability.
     *
     *  Minimal usage:
     *  setPID(p,i,d);
     *  ...looping code...{
     *      output = getOutput(sensorvalue,target);
     *  }
     *
     *  \see https://github.com/tekdemo/MiniPID
     *  \see http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-direction/improving-the-beginners-pid-introduction
     */
    class PIDController {
        private:
            double P = 0.0;
            double I = 0.0;
            double D = 0.0;
            double F = 0.0;

            double maxIOutput = 0.0;
            double maxError = 0.0;
            double errorSum = 0.0;

            double maxOutput = 0.0;
            double minOutput = 0.0;

            double setpoint = 0.0;

            double lastActual = 0.0;

            bool firstRun = true;
            bool reversed = false;

            double outputRampRate = 0.0;
            double lastOutput = 0.0;

            double outputFilter = 0.0;

            double setpointRange = 0.0;

            /**
             *  \brief  Check the signs of the PIDF parameters,
             *          and reverse them if needed.
             */
            void checkSigns() {
                if (this->reversed) {
                    // All values should be below zero
                    if (this->P > 0.0) this->P = -this->P;
                    if (this->I > 0.0) this->I = -this->I;
                    if (this->D > 0.0) this->D = -this->D;
                    if (this->F > 0.0) this->F = -this->F;
                } else {
                    // All values should be above zero
                    if (this->P < 0.0) this->P = -this->P;
                    if (this->I < 0.0) this->I = -this->I;
                    if (this->D < 0.0) this->D = -this->D;
                    if (this->F < 0.0) this->F = -this->F;
                }
            }

        public:
            /**
             *  \brief  Configure PID parameters.
             *  \param  p
             *  \param  i
             *  \param  d
             */
            PIDController(double p, double i, double d)
                : P{p}, I{i}, D{d}, F{0}
            {
                // Empty
            }

            /**
             *  \brief  Configure PID parameters with feedforward parameter.
             *  \param  p
             *  \param  i
             *  \param  d
             */
            PIDController(double p, double i, double d, double f)
                : PIDController(p, i, d)
            {
                this->F = f;
            }

            ~PIDController() = default;

            /**
             *  \brief Configure the Proportional gain parameter.
             *
             *  This responds quicly to changes in setpoint, and provides most
             *  of the initial driving force to make corrections.
             *  Some systems can be used with only a P gain, and many can be operated with only PI.
             *  For position based controllers, this is the first parameter to tune, with I second.
             *  For rate controlled systems, this is often the second after F.
             *
             *  \param p
             *      Proportional gain. Affects output according to
             *      output += P * (setpoint - current_value)
             */
            inline void setP(double p) {
                this->P = p;
                this->checkSigns();
            }

            /**
             *  \brief  Configure the Integral parameter
             *
             *  This is used for overcoming disturbances, and ensuring that the
             *  controller always gets to the control mode.
             *  Typically tuned second for "Position" based modes, and third
             *  for "Rate" or continuous based modes.
             *
             *  Affects output through:
             *  output += previous_errors * Igain;
             *  previous_errors+=current_error</b>
             *
             *  \see {\link #setMaxIOutput(double) setMaxIOutput} for how to restrict
             *  \param  i
             *      New gain value for the Integral term.
             */
            inline void setI(double i) {
                if (this->I != 0.0) {
                    this->errorSum = this->errorSum * this->I / i;
                }

                if (this->maxIOutput != 0.0) {
                    this->maxError = this->maxIOutput / i;
                }

                this->I = i;
                this->checkSigns();
            }

            /**
             *  \brief  Configure the Differential parameter
             *
             *  Responds quickly to large changes in error.
             *  Small values prevents P and I terms from causing overshoot.
             *
             *  \param  d
             *      New gain value for the Differential term.
             */
            inline void setD(double d) {
                this->D = d;
                this->checkSigns();
            }

            /**
             *  \brief  Configure the FeedForward parameter
             *
             *  This is excellent for Velocity, rate, and other	continuous control
             *  modes where you can expect a rough output value based solely
             *  on the setpoint.
             *  Should not be used in "position" based control modes.
             *
             *  \param  f
             *      Feed forward gain. Affects output according to
             *      output += F * Setpoint;
             */
            inline void setF(double f) {
                this->F = f;
                this->checkSigns();
            }

            /**
             *  \brief  Configure PID parameters.
             *
             *  \param  p
             *      Proportional gain. Large if large difference between setpoint and target.
             *  \param  i
             *      Integral gain.	Becomes large if setpoint cannot reach target quickly.
             *  \param  d
             *      Derivative gain. Responds quickly to large changes in error.
             */
            inline void setPID(double p, double i, double d) {
                this->P = p;
                this->I = i;
                this->D = d;
                this->checkSigns();
            }

            /**
             *  \brief  Configure PID parameters with feedforward parameter.
             *
             *  \param  p
             *      Proportional gain. Large if large difference between setpoint and target.
             *  \param  i
             *      Integral gain.	Becomes large if setpoint cannot reach target quickly.
             *  \param  d
             *      Derivative gain. Responds quickly to large changes in error.
             */
            inline void setPID(double p, double i, double d, double f) {
                this->P = p;
                this->I = i;
                this->D = d;
                this->F = f;
                this->checkSigns();
            }

            /**
             *  \brief  Set the maximum output value contributed by the I component of the system.
             *  This can be used to prevent large windup issues and make tuning simpler.
             *
             *  \param  max
             *      Units are the same as the expected output value.
             */
            inline void setMaxIOutput(double max) {
                /* Internally maxError and Izone are similar, but scaled for different purposes.
                 * The maxError is generated for simplifying math, since calculations against
                 * the max error are far more common than changing the I term or Izone.
                 */
                this->maxIOutput = max;

                if (this->I != 0.0) {
                    this->maxError = this->maxIOutput / this->I;
                }
            }

            /**
             *  \brief  Specify a maximum output to [-output, output].
             *
             *  \param  output
             */
            inline void setOutputLimits(double output) {
                this->setOutputLimits(-output, output);
            }

            /**
             *  \brief  Specify a maximum output to [min, max].
             *
             *  \param  min
             *      Minimum possible output value.
             *  \param  max
             *      Maximum possible output value.
             */
            inline void setOutputLimits(double min, double max) {
                if (max < min) return;
                this->minOutput = min;
                this->maxOutput = max;

                const double diff = max - min;

                // Ensure the bounds of the I term are within the bounds of the allowable output swing
                if (this->maxIOutput == 0.0 || this->maxIOutput > diff) {
                    this->setMaxIOutput(diff);
                }
            }

            /**
             *  \brief  Set the operating direction of the PID controller.
             *  \param  reversed
             */
            inline void setDirection(bool reversed) {
                this->reversed = reversed;
            }

            /**
             *  \brief  Set the target value for the PID calculations.
             *  \param  setpoint
             */
            inline void setSetpoint(double setpoint) {
                this->setpoint = setpoint;
            }

            /**
             *  \brief  Resets the controller.
             *          This->erases the I term buildup, and removes D gain on the next loop.
             */
            inline void reset() {
                this->firstRun = true;
                this->errorSum = 0.0;
            }

            /**
             *  \brief  Set the maximum rate the output can increase per cycle.
             *
             *  \param  rate
             */
            inline void setOutputRampRate(double rate) {
                this->outputRampRate = rate;
            }

            /**
             *  \brief  Set a limit on how far the setpoint can be from the current position.
             *
             *  Can simplify tuning by helping tuning over a small range
             *  applies to a much larger range. This limits the reactivity of
             *  P term, and restricts impact of large D term during large setpoint
             *  adjustments. Increases lag and I term if range is too small.
             *
             *  \param  range
             */
            inline void setSetpointRange(double range) {
                this->setpointRange = range;
            }

            /**
             *  \brief  Set a filter on the output to reduce sharp oscillations.
             *
             *  0.1 is likely a sane starting value. Larger values cause P and D
             *  oscillations, but force larger I values. Uses an exponential
             *  rolling sum filter, according to a simple.
             *
             *  \param  strength
             *      output valid between [0..1), meaning [current output only.. historical output only)
             */
            inline void setOutputFilter(double strength) {
                if (strength == 0.0 || utils::math::within(strength, 0.0, 1.0)) {
                    this->outputFilter = strength;
                }
            }

            /**
             *  \brief  Calculates the PID value using the last provided setpoint and actual values.
             *
             *  \return
             *      Calculated output value for driving the actual to the target.
             */
            inline double getOutput() {
                return this->getOutput(this->lastActual, this->setpoint);
            }

            /**
             *  \brief  Calculates the PID value using the last provided setpoint and actual value.
             *
             *  \param  actual
             *      The actual value of the system.
             *  \return
             *      Calculated output value for driving the actual to the target.
             */
            inline double getOutput(double actual){
                return this->getOutput(actual, this->setpoint);
            }

            /**
             *  \brief  Calculate the PID value needed to hit the target setpoint.
             *          Automatically re-calculates the output at each call.
             *
             *  \param  actual
             *      The actual value of the system.
             *  \param  setpoint
             *      The target value of the system.
             *  \return Calculated output value for driving the actual to the target.
             */
            double getOutput(double actual, double setpoint) {
                this->setpoint = setpoint;

                // Ramp the setpoint used for calculations if user has opted to do so
                if (this->setpointRange != 0.0) {
                    setpoint = std::clamp(setpoint,
                                          actual - this->setpointRange,
                                          actual + this->setpointRange);
                }

                // Do the simple parts of the calculations
                const double error = setpoint - actual;

                // Calculate P term
                const double Poutput = this->P * error;

                // Calculate F output. Notice, this->depends only on the setpoint, and not the error.
                const double Foutput = this->F * setpoint;

                // If this->is our first time running this, we don't actually _have_
                // a previous input or output.
                // For sensor, sanely assume it was exactly where it is now.
                // For last output, we can assume it's the current time-independent outputs.
                if (this->firstRun) {
                    this->lastActual = actual;
                    this->lastOutput = Poutput + Foutput;
                    this->firstRun   = false;
                }

                // Interim checks
                const bool  min_out_nequals_max   = utils::math::epsilon_equals(this->minOutput, this->maxOutput, 1e-6);
                const bool  outrampraqte_not_zero = this->outputRampRate != 0.0;
                const bool  max_iout_not_zero     = this->maxIOutput != 0.0;
                const double lastout_min_ramp = this->lastOutput - this->outputRampRate;
                const double lastout_pls_ramp = this->lastOutput + this->outputRampRate;

                // Calculate D Term
                // Note, this is negative. This actually "slows" the system if it's doing
                // the correct thing, and small values helps prevent output spikes and overshoot
                const double Doutput = -this->D * (actual - this->lastActual);
                this->lastActual = actual;

                // The Iterm is more complex. There's several things to factor in to make
                // it easier to deal with.
                //  1. maxIoutput restricts the amount of output contributed by the Iterm.
                //  2. prevent windup by not increasing errorSum if we're already running
                //     against our max Ioutput
                //  3. prevent windup by not increasing errorSum if output is output=maxOutput
                double Ioutput = this->I * this->errorSum;
                if (max_iout_not_zero) {
                    Ioutput = std::clamp(Ioutput, -this->maxIOutput, this->maxIOutput);
                }

                //And, finally, we can just add the terms up
                double output = Foutput + Poutput + Ioutput + Doutput;

                //Figure out what we're doing with the error.
                if (min_out_nequals_max && !utils::math::within(output, this->minOutput, this->maxOutput)) {
                    // reset the error sum to a sane level
                    // Setting to current error ensures a smooth transition when the P term
                    // decreases enough for the I term to start acting upon the controller
                    // From that point the I term will build up as would be expected
                    this->errorSum = error;
                } else if (   outrampraqte_not_zero
                           && !utils::math::within(output, lastout_min_ramp, lastout_pls_ramp))
                {
                    this->errorSum = error;
                } else if (max_iout_not_zero) {
                    // In addition to output limiting directly, we also want to prevent I term
                    // buildup, so restrict the error directly
                    this->errorSum = std::clamp(this->errorSum + error, -this->maxError, this->maxError);
                } else {
                    this->errorSum += error;
                }

                //Restrict output to our specified output and ramp limits
                if (outrampraqte_not_zero) {
                    output = std::clamp(output, lastout_min_ramp, lastout_pls_ramp);
                }

                if (min_out_nequals_max) {
                    output = std::clamp(output, this->minOutput, this->maxOutput);
                }

                if (this->outputFilter != 0.0) {
                    output = utils::math::mix(output, this->lastOutput, this->outputFilter);
                }

                this->lastOutput = output;
                return output;
            }
    };
}

#endif // UTILS_CONTROL_HPP
