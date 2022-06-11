#ifndef _SEQUENCER__H_
#define _SEQUENCER__H_

#include <StateMachine.h>

/**
 * Sequencer
 *
 * This class is useful when some operation needs to be performed with the elements
 * of an array in sequence.  It is intended to be used by stepping through the data
 * elements on separate passes through an Arduino sketch's loop() function.  Because
 * this class inherits from the StateMachine class, the steps can be evenly spaced
 * in time, for operations which must be paced.
 *
 * For example, suppose once a second a series of 5 characters are to be written to the
 * Serial port, one character every 50 milliseconds until all are sent.  This could be
 * done in the following manner.
 *
 *   #include <StateMachine.h>
 *   #include <Sequencer.h>
 *   . . .
 *   const char characters[] = {'A', 'E', 'I', 'O', 'U'};
 *   Sequencer sequencer(50, 5);      // 50 msec, 5 steps per cycle
 *   StateMachine pacer(1000, true);  // repeat every 1000 msec (1 second)
 *   . . .
 *   void loop() {
 *     . . .
 *     // Time to start a sequence?
 *     if (pacer.update()) {
 *       // Yes, start a sequence.
 *       sequencer.start();
 *     }
 *     // Time to advance the stepper if running?
 *     if (sequencer.update()) {
 *       // Yes, is stepper running?
 *       int step = sequencer.read();
 *       if (step >= 0) {
 *         // Yes, send character number 'step' out.
 *         Serial.write(characters[step]);
 *       }
 *     }
 *     . . .
 *   }
 *
 * Author: Thomas W Rackers, June 2022
 * GitHub: https://github.com/twrackers
 * Blog:   https://modelrailroadelectronics.blog
 */

class Sequencer : public StateMachine
{
  private:
    const unsigned int m_nsteps;  // number of steps per cycle
    int m_step;                   // current step number, -1 when not stepping
    bool m_counting;              // true when stepping, false when stopped

  public:
    /**
     * Constructor
     *
     * @param interval time interval between steps, in msec
     * @param nsteps number of steps before sequence ends
     */
    Sequencer(const unsigned int interval, const unsigned int nsteps) :
    StateMachine(interval, true),
    m_nsteps(nsteps), m_step(-1), m_counting(false)
    {
    }

    /**
     * Test for time to update
     *
     * Tests if it is time for the next step to occur.
     *
     * @return true if time for update, false otherwise
     */
    virtual bool update()
    {
      if (StateMachine::update()) {
        if (m_counting) {
          if (++m_step >= m_nsteps) {
            m_step = -1;
            m_counting = false;
          }
        }
        return true;
      }
      return false;
    }

    /**
     * Begin a sequence of steps
     */
    void start(void)
    {
      m_step = 0;
      m_counting = true;
    }

    /**
     * Get current step number
     *
     * @return current step number, from 0 to (nsteps-1) when stepping,
     *      -1 if stopped
     */
    int read() const
    {
      return m_step;
    }
};

#endif
