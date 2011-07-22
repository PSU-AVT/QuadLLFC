/*
 * Author: K Jonathan Harker <kjharke@cs.pdx.edu>
 *
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2011, K Jonathan Harker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the
 * names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <math.h> //sin(), cos()

#include "translate.h"
#include "state.h"

//The Euler Kinematics Equations for transforming from the Body Fixed Frame
//into the Intertial Fixed Frame

void translateB2I(const struct state_controller_t *_bodyState, struct state_controller_t *_inertialState) {
    // Multiply the state vector by a super special rotation matrix:
    // http://www.wolframalpha.com/input/?i={{1%2C0%2C0}%2C{0%2Ccos%28x%29%2Csin%28x%29}%2C{0%2C-sin%28x%29%2Ccos%28x%29}}*{{cos%28y%29%2C0%2C-sin%28y%29}%2C{0%2C1%2C0}%2C{sin%28y%29%2C0%2Ccos%28y%29}}*{{cos%28z%29%2Csin%28z%29%2C0}%2C{-sin%28z%29%2Ccos%28z%29%2C0}%2C{0%2C0%2C1}}
    //    (cos(y) cos(z)                     | cos(y) sin(z)                      | -sin(y)
    //    cos(z) sin(x) sin(y)-cos(x) sin(z) | cos(x) cos(z)+sin(x) sin(y) sin(z) | cos(y) sin(x)
    //    cos(x) cos(z) sin(y)+sin(x) sin(z) | cos(x) sin(y) sin(z)-cos(z) sin(x) | cos(x) cos(y))
    
    // Multiplying the above matrix by [x
    //                                  y
    //                                  z]
    // results in
    //    [ x(cos(y) cos(z)) + y(cos(y) sin(z)) + z(-sin(y))
    //      x(cos(z) sin(x) sin(y)-cos(x) sin(z)) + y(cos(x) cos(z)+sin(x) sin(y) sin(z)) + z(cos(y) sin(x))
    //      x(cos(x) cos(z) sin(y)+sin(x) sin(z)) + y(cos(x) sin(y) sin(z)-cos(z) sin(x)) + z(cos(x) cos(y))]

	/*
    _inertialState->gyros = _bodyState->gyros;
    stateCopy(_bodyState->state_dt, _inertialState->state_dt);

    double R = (double)(_bodyState->state[Roll]);
    double P = (double)(_bodyState->state[Pitch]);
    double Y = (double)(_bodyState->state[Yaw]);

    _inertialState->state[Roll] = (float)(R * cos(P) * cos(Y) + P * cos(P) * sin(Y) + (Y * (0 - sin(P))));
    _inertialState->state[Pitch] = (float)(R * cos(Y) * sin(R) * (sin(P) - cos(R)) * sin(Y) + P * cos(R) * (cos(Y) + sin(R)) * sin(P) * sin(Y) + Y * cos(P) * sin(R));
    _inertialState->state[Yaw] = (float)(R * cos(R) * cos(Y) * (sin(P)+cos(R)) * sin(Y) + P * cos(R) * sin(P) * (sin(Y)-cos(Y)) * sin(R) + Y * cos(R) * cos(P));

    */
}
