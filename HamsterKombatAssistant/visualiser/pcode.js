let prev = null

class obst {
    constructor (a,b,c,d,iskey = false) {
        this.x1 = a
        this.y1 = b
        this.x2 = c
        this.y2 = d
        
        this.iskey = iskey

        if (a == c) {
            this.horizontal = false
            if (this.y1 > this.y2) {
                let temp = this.y1
                this.y1 = this.y2
                this.y2 = temp
            }
        }
        else {
            this.horizontal = true
            if (this.x1 > this.x2) {
                let temp = this.x1
                this.x1 = this.x2
                this.x2 = temp
            }
        }


    }

    fc () {
        if (this.iskey) return "k";
        if (this.horizontal) return "h";
        else return "v";
    }

    tos () {
        return `${this.x1}${this.y1}${this.x2}${this.y2}`
    }
}

let blocks = []

document.addEventListener("DOMContentLoaded", () =>{
    let container = document.querySelector(".container")

    function creategrid() {
        let grid = Array.from(Array(6), () => new Array(6).fill(0));

        for (let i = 0; i < 6; i++) {
            for (let j = 0; j < 6; j++) grid[i][j] = 'e'
        }

        for (let i = 0; i < blocks.length; i++) {
            let obj = blocks[i]
            for (let j = obj.x1; j <= obj.x2; j++) {
                for (let k = obj.y1; k <= obj.y2; k++) {
                    if ((j==obj.x2 && k == obj.y2) || (j==obj.x1 && k == obj.y1)) {
                        if (obj.x1 == obj.x2 && obj.y1 == obj.y2) grid[k][j] = obj.fc() + " rounded"
                        else {
                            if (obj.horizontal) {
                                if (j == obj.x1) grid[k][j] = "lr " +  (obj.iskey ? "k" : "h")
                                if (j == obj.x2) grid[k][j] = "rr " +  (obj.iskey ? "k" : "h")
                            } else {
                                if (k == obj.y1) grid[k][j] = "ur " +  (obj.iskey ? "k" : "v")
                                if (k == obj.y2) grid[k][j] = "dr " +  (obj.iskey ? "k" : "v")
                            }
                        }
                       
                    }else grid[k][j] = "dummy " + obj.fc()
                }
            }
        }

        return grid;
    }   

    function draw() {
        grid = creategrid()
        container.innerHTML = ""
        for (let i = 0; i < 6; i++) {
            let par = container.appendChild(document.createElement("div"))
            for (let j = 0; j < 6; j++) {
                let el = par.appendChild(document.createElement("div"))
                el.classList = grid[i][j];

                el.addEventListener("click", () => { // create an obstacle by clicking on the first and end cells
                    if (prev == null) prev = [j, i]
                    else {
                        console.log(`(${prev[0]},${prev[1]}, ${j},${i})`)
                        blocks.push(new obst(prev[0], prev[1], j, i, blocks.length == 0 ? true : false))
                        prev = null
                        draw()
                    }
                })

            } 
        }
    }

    function ex() {
        let res = ""
        
        for (let i = 0; i < blocks.length; i++) res += blocks[i].tos();

        // navigator.clipboard.writeText(res)
        console.log(res)

        const file = new Blob([res], { type: "text/plain" });
        const url = URL.createObjectURL(file);
        const a = document.createElement("a");
        a.href = url;
        a.download = "HamsterKombatAssistantInitData.txt";
        a.click();

        setTimeout(() => {
            window.close()
        }, 500)
    }

    draw()

    document.querySelector("#btn").addEventListener("click",  () => {ex()})
})
