let init = "02122223"
let moves = "1~0~2;0~1~4;"

let prev = null
let cango = true

class obst {
    constructor (a,b,c,d,iskey = false) {
        this.x1 = a
        this.y1 = b
        this.x2 = c
        this.y2 = d
        this.iskey = iskey

        if (a == c) this.horizontal = false
        else this.horizontal = true
    }

    fc () {
        if (this.iskey) return "k";
        if (this.horizontal) return "h";
        else return "v";
    }

    static stol (s) { // convertation of the key-string to array of obstacles
        let res = []

        for (let i = 0, j = 0; j < s.length - 3; i++, j+=4)
            res.push(new obst(Number(s[j]), Number(s[j+1]), Number(s[j+2]), Number(s[j+3]), i==0))

        return res;
    }
}

let blocks = []
let actions = []

blocks = obst.stol(init)

console.log(init)
console.log(moves)

moves.split(";").forEach(el => {
    actions.push({id: Number(el.split("~")[0]), to: Number(el.split("~")[1]), len: Number(el.split("~")[2])})
})

document.addEventListener("DOMContentLoaded", () =>{
    


    let container = document.querySelector(".container")

    function creategrid() {
        let grid = Array.from(Array(6), () => new Array(6).fill(0));

        for (let i = 0; i < 6; i++) {
            for (let j = 0; j < 6; j++) grid[i][j] = 'e'
        }

        // console.log(blocks)

        for (let i = 0; i < blocks.length; i++) {
            let obj = blocks[i]
            // console.log(grid)
            for (let j = obj.x1; j <= obj.x2; j++) {
                for (let k = obj.y1; k <= obj.y2; k++) {
                    if ((j==obj.x2 && k == obj.y2) || (j==obj.x1 && k == obj.y1)) {
                        if (obj.x1 == obj.x2 && obj.y1 == obj.y2) grid[k][j] = "rounded "  + obj.fc()
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
        // console.log(grid)
        container.innerHTML = ""
        for (let i = 0; i < 6; i++) {
            let par = container.appendChild(document.createElement("div"))
            for (let j = 0; j < 6; j++) {
                let el = par.appendChild(document.createElement("div"))
                el.classList = grid[i][j];
                el.addEventListener("click", () => {
                    if (prev == null) prev = [j, i]
                    else {
                        console.log(`(${prev[0]},${prev[1]}, ${j},${i})`)
                        prev = null
                    }
                })
            } 
        }
    }
    
    let index = 0
    
    const clamp = (v, mn, mx) => Math.min(Math.max(v, mn), mx)

    function move(back = false) {
        if (!cango) return

        cof = (back ? -1 : 1) * (actions[index].to == 3 || actions[index].to == 0 ? -1 : 1)   
        let counter = 0
        cango = false

        let timer = setInterval(() => {
            if (counter < actions[index].len) {
                if (actions[index].to % 2 == 1) {
                    blocks[actions[index].id].x1 = clamp(blocks[actions[index].id].x1+cof, 0, 5)
                    blocks[actions[index].id].x2 = clamp(blocks[actions[index].id].x2+cof, 0, 5)
                }else {
                    blocks[actions[index].id].y1 = clamp(blocks[actions[index].id].y1+cof, 0, 5)
                    blocks[actions[index].id].y2 = clamp(blocks[actions[index].id].y2+cof, 0, 5)
                }
                
                draw()
                counter++
            }else {
                cango = true
                clearInterval(timer)
                if (!back) index = clamp(index+1, 0, actions.length-1)
            }
        }, 50)
    }

    draw()

    document.querySelector("#btn").addEventListener("click",  () => {
        if (cango) move()
    })

    document.querySelector("#btn2").addEventListener("click",  () => {
        if (cango) {
            if (index > 0) {
                index--;
                move(true)
            }
        }
    })
})
