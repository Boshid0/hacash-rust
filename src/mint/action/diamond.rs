
/**
 * Diamond Transfer
 */
 ActionDefine!{
    DiamondSingleTransfer : 5, (
        diamond  : DiamondName
        to       : AddrOrPtr
    ),
    ACTLV_MAIN, // level
    6 + 21, // gas
    (self, ctx, state, store, gas), // params
    false, // burn 90
    [], // req sign
    diamond_transfer(self, ctx, state, store)
}

fn diamond_transfer(this: &DiamondSingleTransfer, ctx: &mut dyn ExecContext, sta: &mut dyn State, sto: &dyn Store) -> Ret<Vec<u8>> {
    let from = ctx.main_address().clone();
    let to = this.to.real(ctx.addr_list())?;
    let mut diamonds = DiamondNameListMax200::default();
    diamonds.push(this.diamond);
    do_diamonds_transfer(&diamonds, from, to, ctx, sta, sto)
}


/**
 * Diamond Multiple From Transfer
 */
 ActionDefine!{
    DiamondFromTransfer : 8, (
        from           : AddrOrPtr 
        diamonds       : DiamondNameListMax200 
    ),
    ACTLV_MAIN, // level
    21, // gas
    (self, ctx, state, store, gas), // params
    false, // burn 90
    [self.from], // req sign
    {
        // gas = dia num * 6
        gas += self.diamonds.count().uint() as i64 * DiamondName::width() as i64;
        diamond_from_transfer(self, ctx, state, store)
    }
}

fn diamond_from_transfer(this: &DiamondFromTransfer, ctx: &mut dyn ExecContext, sta: &mut dyn State, sto: &dyn Store) -> Ret<Vec<u8>> {
    let from = this.from.real(ctx.addr_list())?;
    let to = ctx.main_address().clone();
    do_diamonds_transfer(&this.diamonds, from, to, ctx, sta, sto)
}


/**
 * Diamond Multiple From To Transfer
 */
 ActionDefine!{
    DiamondFromToTransfer : 6, (
        from           : AddrOrPtr 
        to             : AddrOrPtr 
        diamonds       : DiamondNameListMax200 
    ),
    ACTLV_MAIN, // level
    21+21, // gas
    (self, ctx, state, store, gas), // params
    false, // burn 90
    [self.from], // req sign
    {
        // gas = dia num * 6
        gas += self.diamonds.count().uint() as i64 * DiamondName::width() as i64;
        diamond_from_to_transfer(self, ctx, state, store)
    }
}

fn diamond_from_to_transfer(this: &DiamondFromToTransfer, ctx: &mut dyn ExecContext, sta: &mut dyn State, sto: &dyn Store) -> Ret<Vec<u8>> {
    let from = this.from.real(ctx.addr_list())?;
    let to = this.to.real(ctx.addr_list())?;
    do_diamonds_transfer(&this.diamonds, from, to, ctx, sta, sto)
}




/**
 * Diamond Multipl Transfer
 */
 ActionDefine!{
    DiamondToTransfer : 7, (
        to             : AddrOrPtr
        diamonds       : DiamondNameListMax200 
    ),
    ACTLV_MAIN, // level
    21, // gas
    (self, ctx, state, store, gas), // params
    false, // burn 90
    [], // req sign
    {
        // gas = dia num * 6
        gas += self.diamonds.count().uint() as i64 * DiamondName::width() as i64;
        diamond_to_transfer(self, ctx, state, store)
    }
}

fn diamond_to_transfer(this: &DiamondToTransfer, ctx: &mut dyn ExecContext, sta: &mut dyn State, sto: &dyn Store) -> Ret<Vec<u8>> {
    let from = ctx.main_address().clone();
    let to = this.to.real(ctx.addr_list())?;
    do_diamonds_transfer(&this.diamonds, from, to, ctx, sta, sto)
}



/////////////////////////



fn do_diamonds_transfer(diamonds: &DiamondNameListMax200, from: Address, to: Address, ctx: &mut dyn ExecContext, sta: &mut dyn State, sto: &dyn Store) -> Ret<Vec<u8>> {
    // check
    let dianum = diamonds.check()?;
    //transfer
    let mut state = MintState::wrap(sta);
    for dianame in diamonds.list() {
        hacd_move_one_diamond(&mut state, &from, &to, &dianame)?; // move one
    }
    diamond_owned_move(&mut state, &from, &to, diamonds)?;
    drop(state);
    // transfer
    let mut core_state = CoreState::wrap(sta);
    hacd_transfer(ctx, &mut core_state, &from, &to, &DiamondNumber::from(dianum as u32), &diamonds)
}


