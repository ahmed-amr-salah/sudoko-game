<template>
  <div class="sudoku-board">
    <v-card class="ma-4 main-card" elevation="8">
      <v-card-title class="text-h4 py-4 title-gradient">
        <v-icon large class="mr-2">mdi-grid</v-icon>
        <span>Sudoku Solver</span>
        <v-spacer></v-spacer>
        <v-btn 
          color="accent" 
          @click="solvePuzzle" 
          :loading="solving" 
          :disabled="!puzzleLoaded"
          class="solve-btn"
          elevation="4"
        >
          <v-icon left>mdi-lightning-bolt</v-icon>
          <span>Solve</span>
        </v-btn>
      </v-card-title>
      
      <!-- File Input Section - HIGHLIGHTED FOR VISIBILITY -->
      <v-card-subtitle class="pt-4">
        <v-alert
          type="info"
          outlined
          class="mb-4 upload-alert"
        >
          <strong>Upload a Sudoku puzzle file or select a sample puzzle below</strong>
        </v-alert>
        
        <v-row align="center">
          <v-col cols="12" md="6">
            <v-file-input
              v-model="file"
              label="Upload Sudoku Puzzle"
              accept=".txt"
              @change="uploadFile"
              prepend-icon="mdi-upload"
              outlined
              hint="Upload a .txt file with your Sudoku puzzle"
              persistent-hint
              color="primary"
              class="upload-input"
              show-size
              truncate-length="15"
            ></v-file-input>
          </v-col>
          <v-col cols="12" md="6">
            <v-select
              v-model="selectedSample"
              label="Or try a sample puzzle"
              :items="samplePuzzles"
              @update:model-value="loadSamplePuzzle"
              outlined
              hint="Select a pre-defined puzzle to try"
              persistent-hint
              color="primary"
              class="sample-select"
            ></v-select>
          </v-col>
        </v-row>
      </v-card-subtitle>
      
      <!-- Status Message -->
      <v-card-text v-if="statusMessage" class="status-message">
        <v-alert
          :type="statusSuccess ? 'success' : 'error'"
          dense
          transition="scale-transition"
          class="status-alert"
        >
          {{ statusMessage }}
        </v-alert>
      </v-card-text>
      
      <!-- Puzzle Display Section -->
      <v-card-text>
        <div class="d-flex flex-wrap justify-center">
          <!-- Original puzzle -->
          <v-card class="board-container ma-2" elevation="3">
            <v-card-title class="board-title">
              <v-icon left>mdi-puzzle-outline</v-icon>
              <span>Original Puzzle</span>
            </v-card-title>
            <v-card-text>
              <div class="sudoku-grid">
                <div v-for="(row, rowIndex) in puzzle" :key="`orig-${rowIndex}`" class="sudoku-row">
                  <div 
                    v-for="(cell, colIndex) in row" 
                    :key="`orig-${rowIndex}-${colIndex}`" 
                    class="sudoku-cell"
                    :class="{ 'filled': cell > 0 }"
                  >
                    {{ cell > 0 ? cell : '' }}
                  </div>
                </div>
              </div>
            </v-card-text>
          </v-card>
          
          <!-- Solution -->
          <v-card class="board-container ma-2" elevation="3">
            <v-card-title class="board-title">
              <v-icon left>mdi-check-decagram</v-icon>
              <span>Solution</span>
            </v-card-title>
            <v-card-text>
              <div class="sudoku-grid">
                <div v-for="(row, rowIndex) in solution" :key="`sol-${rowIndex}`" class="sudoku-row">
                  <div 
                    v-for="(cell, colIndex) in row" 
                    :key="`sol-${rowIndex}-${colIndex}`" 
                    class="sudoku-cell"
                    :class="{ 
                      'filled': cell > 0,
                      'original': puzzle[rowIndex][colIndex] > 0,
                      'solved': cell > 0 && puzzle[rowIndex][colIndex] === 0
                    }"
                  >
                    <transition name="flip" mode="out-in">
                      <span :key="cell">{{ cell > 0 ? cell : '' }}</span>
                    </transition>
                  </div>
                </div>
              </div>
            </v-card-text>
          </v-card>
        </div>
      </v-card-text>
      
      <!-- Instructions -->
      <v-card-text>
        <v-expansion-panels class="instruction-panel">
          <v-expansion-panel>
            <v-expansion-panel-title>
              <v-icon left>mdi-information-outline</v-icon>
              <span>Instructions</span>
            </v-expansion-panel-title>
            <v-expansion-panel-text>
              <p>Upload a Sudoku puzzle file or select a sample puzzle, then click "Solve" to solve the puzzle.</p>
              <p>The file should be a text file with 9 rows and 9 columns, where each cell is represented by a digit (1-9) or 0/./space for empty cells.</p>
              <p>Example format:</p>
              <pre class="example-format">
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
8 0 0 0 6 0 0 0 3
4 0 0 8 0 3 0 0 1
7 0 0 0 2 0 0 0 6
0 6 0 0 0 0 2 8 0
0 0 0 4 1 9 0 0 5
0 0 0 0 8 0 0 7 9
              </pre>
            </v-expansion-panel-text>
          </v-expansion-panel>
        </v-expansion-panels>
      </v-card-text>
    </v-card>
  </div>
</template>

<script>
export default {
  name: 'SudokuBoard',
  
  data() {
    return {
      puzzle: Array(9).fill().map(() => Array(9).fill(0)),
      solution: Array(9).fill().map(() => Array(9).fill(0)),
      file: null,
      selectedSample: null,
      samplePuzzles: [
        { title: 'Easy', value: 'easy' },
        { title: 'Medium', value: 'medium' },
        { title: 'Hard', value: 'hard' }
      ],
      solving: false,
      statusMessage: '',
      statusSuccess: true,
      puzzleLoaded: false,
      pollInterval: null
    }
  },
  
  mounted() {
    // Start polling for messages
    this.startPolling();
  },
  
  beforeUnmount() {
    // Stop polling when component is destroyed
    this.stopPolling();
  },
  
  methods: {
    startPolling() {
      // Poll for messages every second
      this.pollInterval = setInterval(() => {
        this.pollMessages();
      }, 1000);
    },
    
    stopPolling() {
      if (this.pollInterval) {
        clearInterval(this.pollInterval);
        this.pollInterval = null;
      }
    },
    
    async pollMessages() {
      try {
        const response = await fetch('/api/messages');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const messages = await response.json();
        
        if (messages.length > 0) {
          for (const message of messages) {
            this.handleMessage(message);
          }
        }
      } catch (error) {
        console.error('Error polling messages:', error);
      }
    },
    
    handleMessage(message) {
      if (message.type === 'puzzle') {
        this.puzzle = message.grid;
        this.puzzleLoaded = true;
        this.statusMessage = 'Puzzle loaded successfully';
        this.statusSuccess = true;
      } else if (message.type === 'solution') {
        this.solution = message.grid;
        this.solving = false;
      } else if (message.type === 'status') {
        this.statusMessage = message.message;
        this.statusSuccess = message.success;
        this.solving = false;
      } else if (message.type === 'error') {
        this.statusMessage = message.message;
        this.statusSuccess = false;
        this.solving = false;
      }
    },
    
    async uploadFile() {
      if (!this.file) return;
      
      try {
        const reader = new FileReader();
        
        reader.onload = async (e) => {
          const content = e.target.result;
          
          // Send the file content to the server
          const response = await fetch('/api/puzzle', {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json'
            },
            body: JSON.stringify({
              type: 'file',
              filename: this.file.name,
              content: content
            })
          });
          
          if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
          }
          
          this.statusMessage = 'File uploaded, loading puzzle...';
          this.statusSuccess = true;
        };
        
        reader.readAsText(this.file);
      } catch (error) {
        console.error('Error uploading file:', error);
        this.statusMessage = `Error uploading file: ${error.message}`;
        this.statusSuccess = false;
      }
    },
    
    async solvePuzzle() {
      if (!this.puzzleLoaded) {
        this.statusMessage = 'Please load a puzzle first';
        this.statusSuccess = false;
        return;
      }
      
      this.solving = true;
      this.statusMessage = 'Solving puzzle...';
      this.statusSuccess = true;
      
      try {
        const response = await fetch('/api/puzzle', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            type: 'solve'
          })
        });
        
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
      } catch (error) {
        console.error('Error solving puzzle:', error);
        this.statusMessage = `Error solving puzzle: ${error.message}`;
        this.statusSuccess = false;
        this.solving = false;
      }
    },
    
    async loadSamplePuzzle(puzzleType) {
      if (!puzzleType) return;
      
      this.statusMessage = `Loading ${puzzleType} sample puzzle...`;
      this.statusSuccess = true;
      
      try {
        // Define sample puzzle content directly
        let content = '';
        
        switch (puzzleType) {
          case 'easy':
            content = `
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
8 0 0 0 6 0 0 0 3
4 0 0 8 0 3 0 0 1
7 0 0 0 2 0 0 0 6
0 6 0 0 0 0 2 8 0
0 0 0 4 1 9 0 0 5
0 0 0 0 8 0 0 7 9
            `;
            break;
          case 'medium':
            content = `
0 0 0 2 6 0 7 0 1
6 8 0 0 7 0 0 9 0
1 9 0 0 0 4 5 0 0
8 2 0 1 0 0 0 4 0
0 0 4 6 0 2 9 0 0
0 5 0 0 0 3 0 2 8
0 0 9 3 0 0 0 7 4
0 4 0 0 5 0 0 3 6
7 0 3 0 1 8 0 0 0
            `;
            break;
          case 'hard':
            content = `
0 2 0 0 0 0 0 0 0
0 0 0 6 0 0 0 0 3
0 7 4 0 8 0 0 0 0
0 0 0 0 0 3 0 0 2
0 8 0 0 4 0 0 1 0
6 0 0 5 0 0 0 0 0
0 0 0 0 1 0 7 8 0
5 0 0 0 0 9 0 0 0
0 0 0 0 0 0 0 4 0
            `;
            break;
          default:
            throw new Error(`Unknown puzzle type: ${puzzleType}`);
        }
        
        // Send the puzzle content directly to the server
        const uploadResponse = await fetch('/api/puzzle', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({
            type: 'file',
            filename: `sample_${puzzleType}.txt`,
            content: content
          })
        });
        
        if (!uploadResponse.ok) {
          throw new Error(`HTTP error! status: ${uploadResponse.status}`);
        }
      } catch (error) {
        console.error('Error loading sample puzzle:', error);
        this.statusMessage = `Error loading sample puzzle: ${error.message}`;
        this.statusSuccess = false;
      }
    }
  }
}
</script>

<style scoped>
.main-card {
  border-radius: 16px;
  overflow: hidden;
  box-shadow: 0 8px 30px rgba(0, 0, 0, 0.1);
}

.title-gradient {
  background: linear-gradient(135deg, #6a11cb 0%, #2575fc 100%);
  color: white;
  padding: 20px !important;
}

.solve-btn {
  transition: transform 0.3s ease;
}

.solve-btn:hover {
  transform: translateY(-3px);
}

.board-container {
  flex: 1;
  min-width: 300px;
  max-width: 450px;
  border-radius: 12px;
  overflow: hidden;
  transition: transform 0.3s ease, box-shadow 0.3s ease;
}

.board-container:hover {
  transform: translateY(-5px);
  box-shadow: 0 12px 30px rgba(0, 0, 0, 0.15);
}

.board-title {
  background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
  color: white;
  font-size: 1.2rem;
  padding: 12px 16px;
}

.sudoku-grid {
  display: flex;
  flex-direction: column;
  border: 2px solid #333;
  width: 100%;
  max-width: 450px;
  margin: 0 auto;
  box-shadow: 0 4px 20px rgba(0, 0, 0, 0.1);
}

.sudoku-row {
  display: flex;
  height: calc(100% / 9);
}

.sudoku-row:nth-child(3n) {
  border-bottom: 2px solid #333;
}

.sudoku-row:last-child {
  border-bottom: none;
}

.sudoku-cell {
  flex: 1;
  aspect-ratio: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid #ccc;
  font-size: 1.5rem;
  font-weight: bold;
  transition: background-color 0.3s ease;
}

.sudoku-cell:nth-child(3n) {
  border-right: 2px solid #333;
}

.sudoku-cell:last-child {
  border-right: none;
}

.sudoku-cell.filled {
  background-color: rgba(106, 17, 203, 0.1);
}

.sudoku-cell.original {
  color: #6a11cb;
  font-weight: 900;
}

.sudoku-cell.solved {
  color: #11998e;
  animation: fadeIn 0.5s ease-in-out;
}

.instruction-panel {
  border-radius: 8px;
  overflow: hidden;
  box-shadow: 0 4px 15px rgba(0, 0, 0, 0.05);
}

.example-format {
  background-color: #f8f9fa;
  padding: 15px;
  border-radius: 8px;
  font-family: monospace;
  white-space: pre-wrap;
  border-left: 4px solid #6a11cb;
}

.status-alert {
  border-radius: 8px;
  margin-bottom: 16px;
}

.upload-input, .sample-select {
  border-radius: 8px;
}

/* Animations */
@keyframes fadeIn {
  from { opacity: 0; }
  to { opacity: 1; }
}

.flip-enter-active, .flip-leave-active {
  transition: transform 0.5s, opacity 0.5s;
}

.flip-enter-from, .flip-leave-to {
  transform: rotateY(90deg);
  opacity: 0;
}
</style> 