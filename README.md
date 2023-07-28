<h1>SerialRealTime</h1>

<h2>SerialRealTime : Class to create real-time writer serial on Qt using QThread technology</h2>

*Apache License 2.0 with Commons Clause*
*Copyright 2023 Miailhe Paul, all rights reserved.*

<p>To be able to control a measuring bench using the GRBL system, I wrote a serial class that allows to send commands to manage the movement. 
        Attention, I do not manage the reading, being messages of non-fixed sizes it would be necessary to remodel the whole code to be able to read.</p>
        
        - class MainWindow : Manages the display and user signals.
        - Class SerialPort : Class managing the serial port and sending messages using another thread than the main one

## Commons Clause

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

The Software is provided to you by the Licensor under the License, as defined below, subject to the following condition.
Without limiting other conditions in the License, the grant of rights under the License will not include, and the License does not grant to you, the right to Sell the Software.
For purposes of the foregoing, “Sell” means practicing any or all of the rights granted to you under the License to provide to third parties, for a fee or other consideration (including without limitation fees for hosting or consulting/ support services related to the Software), a product or service whose value derives, entirely or substantially, from the functionality of the Software.
