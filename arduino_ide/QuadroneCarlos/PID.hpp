#ifndef _PID_
#define _PID_

class PID {

  private:

    int _in_min,
        _in_max,
        _out_min,
        _out_max;

    float _k_p,
          _k_i,
          _k_d;

    int err,
        last_err,
        delta_err;

    int P, I, D;

    unsigned long current_time,
             previous_time,
             elapsed_time;

  public:

    PID(float k_p,
        float k_i,
        float k_d,
        int in_min,
        int in_max,
        int out_min,
        int out_max) :
      _k_p(k_p),
      _k_i(k_i),
      _k_d(k_d),
      _in_min(in_min),
      _in_max(in_max),
      _out_min(out_min),
      _out_max(out_max) { }

    int evaluate(int input, int reference) {

      err = reference - input;

      delta_err = err - last_err;

      int output = evaluate(delta_err);

      last_err = err;

      return output;
    }

    int evaluate(int input, int reference, int delta_err) {

      err = reference - input;

      return evaluate(delta_err);
    }

    int evaluate(int delta_err) {

      current_time = millis();
      elapsed_time = current_time - previous_time;
      previous_time = current_time;

      if (elapsed_time) {
        P = _k_p * (float)err;
        I = _k_i == 0 ? 0 : constrain(I + (_k_i * (float)err * (float)elapsed_time), _in_min, _in_max);
        D = _k_d * (float)delta_err / (float)elapsed_time;
      }

      return constrain(
               map(P + I + D, _in_min, _in_max, _out_min, _out_max),
               _out_min,
               _out_max
             );
    }

    void reset(void) {
      P = 0;
      I = 0;
      D = 0;

      err = 0;
      last_err = 0;
      delta_err = 0;

      current_time = 0;
      previous_time = 0;
      elapsed_time = 0;
    }

    void set_coef(float k_p, float k_i, float k_d) {
      _k_p = k_p;
      _k_i = k_i;
      _k_d = k_d;
    }

    void set_k_p(float k_p) {
      _k_p = k_p;
    }

    void set_k_i(float k_i) {
      _k_i = k_i;
    }

    void set_k_d(float k_d) {
      _k_d = k_d;
    }

};

#endif
