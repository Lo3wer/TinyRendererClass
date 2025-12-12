Process for triangle rasterization:
- Get top middle bottom points
- Draw horizontal lines from top to bottm with the wireframe acting as bounds
- Split triangle in half or have a condition in the loop that checks which half its in

Integer division works better than the float t calculation alternative shown in rastWithFloat